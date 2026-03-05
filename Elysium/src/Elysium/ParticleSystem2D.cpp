#include "ParticleSystem2D.h"

#include <execution>

#include <glm/gtc/constants.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/compatibility.hpp>

#include "Elysium/Log.h"
#include "Elysium/Random.h"

namespace Elysium
{
	static ParticleSystem2D* s_Instance = nullptr;

	ParticleSystem2D::ParticleSystem2D(
		size_t poolSize
	) :
		m_ParticlePoolSize(poolSize),
		m_PoolIndex(0),
		m_ComputeShader("res/shaders/particles.glsl", ShaderType::COMPUTE)
	{
		glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &m_WorkGroupSize);
#ifdef _DEBUG
		ELY_CORE_INFO("Work Group Size: {0}", m_WorkGroupSize);
#endif
		resizePool(poolSize);
	}

	ParticleSystem2D::~ParticleSystem2D()
	{
		GL_ASSERT(glDeleteBuffers(s_BufferCount, m_SSBO.data()));
	}

	void ParticleSystem2D::reset(size_t poolSize)
	{
		applyOnVectors(
			[&](auto& vec)
			{
				vec.clear();
			}
		);

		resizePool(poolSize);

		m_PoolIndex = 0;
		m_Warning = false;
	}

	void ParticleSystem2D::Init(size_t poolSize)
	{
		if (!s_Instance) s_Instance = new ParticleSystem2D(poolSize);
	}

	void ParticleSystem2D::Shutdown()
	{
		delete s_Instance;
		s_Instance = nullptr;
	}

	ParticleSystem2D& ParticleSystem2D::Get()
	{
		return *s_Instance;
	}

	void ParticleSystem2D::resizePool(size_t poolSize)
	{
		m_ParticlePoolSize = poolSize;

		applyOnVectors(
			[&](auto& vec)
			{
				vec.resize(m_ParticlePoolSize);
			}
		);

		GL_ASSERT(glGenBuffers(s_BufferCount, m_SSBO.data()));
		for (int i = 0; i < s_BufferCount; ++i)
		{
			GL_ASSERT(glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_SSBO[i]));
			switch(i)
			{
			case 7:
			case 8:
			case 9:
			{
				GL_ASSERT(glBufferData(GL_SHADER_STORAGE_BUFFER, m_ParticlePoolSize * sizeof(Vector4), NULL, GL_STATIC_DRAW));
				break;
			}
			default:
			{
				GL_ASSERT(glBufferData(GL_SHADER_STORAGE_BUFFER, m_ParticlePoolSize * sizeof(float), NULL, GL_STATIC_DRAW));
			}
			}
			GL_ASSERT(glBindBufferBase(GL_SHADER_STORAGE_BUFFER, i, m_SSBO[i]));
		}
	}

	void ParticleSystem2D::addParticle(const ParticleProperties& particleProperties, size_t index)
	{
		m_Position[index] = particleProperties.Position;
		m_Rotation[index] = Random::Float() * 360.0f;
		m_RotationSpeed[index] = Random::Float() * particleProperties.RotationSpeed;

		m_Velocity[index] = particleProperties.Velocity;
		m_Velocity[index].x += particleProperties.VelocityVariation.x * (Random::Float() - 0.5f);
		m_Velocity[index].y += particleProperties.VelocityVariation.y * (Random::Float() - 0.5f);

		m_ColorBegin[index] = particleProperties.ColorBegin;
		m_ColorEnd[index] = particleProperties.ColorEnd;

		m_SizeBegin[index] = particleProperties.SizeBegin + particleProperties.SizeVariation * (Random::Float() - 0.5f);
		m_SizeEnd[index] = particleProperties.SizeEnd;

		m_LifeTime[index] = particleProperties.LifeTime;
		m_LifeRemaining[index] = particleProperties.LifeTime;

		m_Active[index] = true;
	}

	template<>
	void ParticleSystem2D::onUpdate<UpdateDevice::CPU>(Timestep ts)
	{
		float timestep = static_cast<float>(ts);
		for (size_t i = 0; i < m_PoolIndex; ++i)
		{
			m_LifeRemaining[i] -= timestep;
			m_Position[i] += m_Velocity[i] * timestep;
			m_Rotation[i] += m_RotationSpeed[i] * timestep;

			// Fade away particles
			float lifeRatio = m_LifeRemaining[i] / m_LifeTime[i];
			m_Color[i] = glm::lerp(m_ColorEnd[i], m_ColorBegin[i], lifeRatio);
			m_Color[i].a = lifeRatio;

			m_Size[i] = glm::lerp(m_SizeEnd[i], m_SizeBegin[i], lifeRatio);
		}
	}

	template<>
	void ParticleSystem2D::onUpdate<UpdateDevice::GPU>(Timestep ts)
	{
		GL_ASSERT(glNamedBufferSubData(m_SSBO[0], 0, m_PoolIndex * sizeof(float), m_Position.data()));
		GL_ASSERT(glNamedBufferSubData(m_SSBO[1], 0, m_PoolIndex * sizeof(float), m_Velocity.data()));

		GL_ASSERT(glNamedBufferSubData(m_SSBO[2], 0, m_PoolIndex * sizeof(float), m_Rotation.data()));
		GL_ASSERT(glNamedBufferSubData(m_SSBO[3], 0, m_PoolIndex * sizeof(float), m_RotationSpeed.data()));

		GL_ASSERT(glNamedBufferSubData(m_SSBO[4], 0, m_PoolIndex * sizeof(float), m_SizeBegin.data()));
		GL_ASSERT(glNamedBufferSubData(m_SSBO[5], 0, m_PoolIndex * sizeof(float), m_SizeEnd.data()));
		GL_ASSERT(glNamedBufferSubData(m_SSBO[6], 0, m_PoolIndex * sizeof(float), m_Size.data()));

		GL_ASSERT(glNamedBufferSubData(m_SSBO[7], 0, m_PoolIndex * sizeof(Vector4), m_ColorBegin.data()));
		GL_ASSERT(glNamedBufferSubData(m_SSBO[8], 0, m_PoolIndex * sizeof(Vector4), m_ColorEnd.data()));
		GL_ASSERT(glNamedBufferSubData(m_SSBO[9], 0, m_PoolIndex * sizeof(Vector4), m_Color.data()));

		GL_ASSERT(glNamedBufferSubData(m_SSBO[10], 0, m_PoolIndex * sizeof(float), m_LifeTime.data()));
		GL_ASSERT(glNamedBufferSubData(m_SSBO[11], 0, m_PoolIndex * sizeof(float), m_LifeRemaining.data()));

		m_ComputeShader.bind();
		m_ComputeShader.setUniform1f("u_Timestep", static_cast<float>(ts));
		GL_ASSERT(glDispatchCompute((m_PoolIndex / m_WorkGroupSize) + 1, 1, 1));
		GL_ASSERT(glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT));

		GL_ASSERT(glGetNamedBufferSubData(m_SSBO[0], 0, m_PoolIndex * sizeof(float), m_Position.data()));
		GL_ASSERT(glGetNamedBufferSubData(m_SSBO[1], 0, m_PoolIndex * sizeof(float), m_Velocity.data()));

		GL_ASSERT(glGetNamedBufferSubData(m_SSBO[2], 0, m_PoolIndex * sizeof(float), m_Rotation.data()));
		GL_ASSERT(glGetNamedBufferSubData(m_SSBO[3], 0, m_PoolIndex * sizeof(float), m_RotationSpeed.data()));

		GL_ASSERT(glGetNamedBufferSubData(m_SSBO[4], 0, m_PoolIndex * sizeof(float), m_SizeBegin.data()));
		GL_ASSERT(glGetNamedBufferSubData(m_SSBO[5], 0, m_PoolIndex * sizeof(float), m_SizeEnd.data()));
		GL_ASSERT(glGetNamedBufferSubData(m_SSBO[6], 0, m_PoolIndex * sizeof(float), m_Size.data()));

		GL_ASSERT(glGetNamedBufferSubData(m_SSBO[7], 0, m_PoolIndex * sizeof(Vector4), m_ColorBegin.data()));
		GL_ASSERT(glGetNamedBufferSubData(m_SSBO[8], 0, m_PoolIndex * sizeof(Vector4), m_ColorEnd.data()));
		GL_ASSERT(glGetNamedBufferSubData(m_SSBO[9], 0, m_PoolIndex * sizeof(Vector4), m_Color.data()));

		GL_ASSERT(glGetNamedBufferSubData(m_SSBO[10], 0, m_PoolIndex * sizeof(float), m_LifeTime.data()));
		GL_ASSERT(glGetNamedBufferSubData(m_SSBO[11], 0, m_PoolIndex * sizeof(float), m_LifeRemaining.data()));
	}

	void ParticleSystem2D::Emit(const ParticleProperties& particleProperties)
	{
		if (!m_Active[m_PoolIndex] && m_PoolIndex < m_ParticlePoolSize)
		{
			addParticle(particleProperties, m_PoolIndex);
			m_ParticleTextureData[m_PoolIndex++] = particleProperties.TextureData;
		}
		else
		{
			if (!m_Warning)
			{
				ELY_CORE_WARN("Increase Pool Size!");
				m_Warning = true;
			}
		}
	}

	template<UpdateDevice U>
	void ParticleSystem2D::onUpdate(Timestep ts)
	{
		onUpdate<U>(ts);
	}

	void ParticleSystem2D::onRender(const OrthographicCamera& camera)
	{
		Renderer::beginScene(camera);
		for (size_t i = 0; i < m_PoolIndex; ++i)
		{
			if (m_LifeRemaining[i] <= 0.0f)
			{
				m_Active[i] = false;

				--m_PoolIndex;
				std::swap(m_Position[i], m_Position[m_PoolIndex]);
				std::swap(m_Velocity[i], m_Velocity[m_PoolIndex]);
				std::swap(m_Rotation[i], m_Rotation[m_PoolIndex]);
				std::swap(m_RotationSpeed[i], m_RotationSpeed[m_PoolIndex]);
				std::swap(m_SizeBegin[i], m_SizeBegin[m_PoolIndex]);
				std::swap(m_Size[i], m_Size[m_PoolIndex]);
				std::swap(m_ColorBegin[i], m_ColorBegin[m_PoolIndex]);
				std::swap(m_ColorEnd[i], m_ColorEnd[m_PoolIndex]);
				std::swap(m_Color[i], m_Color[m_PoolIndex]);
				std::swap(m_LifeTime[i], m_LifeTime[m_PoolIndex]);
				std::swap(m_LifeRemaining[i], m_LifeRemaining[m_PoolIndex]);
				std::swap(m_Active[i], m_Active[m_PoolIndex]);
				std::swap(m_ParticleTextureData[i], m_ParticleTextureData[m_PoolIndex]);
				continue;
			}

			TextureData particleTextureData = m_ParticleTextureData[i];
			// RendererID = .RendererID > 1 ? RendererID : 1;
			particleTextureData.RendererID +=  1 + ( m_ParticleTextureData[i].RendererID  - 1 ) * ( m_ParticleTextureData[i].RendererID > 1 );

			Renderer::drawQuadWithRotation(
				m_Position[i],
				{ m_Size[i], m_Size[i] },
				glm::radians(m_Rotation[i]),
				particleTextureData,
				{ 1.0f, 1.0f },
				m_Color[i]
			);
		}
		Renderer::endScene();
	}
}