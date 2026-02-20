#include "ParticleSystem2D.h"

#include <execution>

#include <glm/gtc/constants.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/compatibility.hpp>

#include "Elysium/Random.h"

#include "Elysium/Log.h"

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
		GL_ASSERT(glDeleteBuffers(1, &m_SSBO));
	}

	void ParticleSystem2D::reset(size_t poolSize)
	{
		m_ParticlePool.clear();
		m_ParticleTextureData.clear();

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
		m_ParticlePool.resize(poolSize);
		m_ParticleTextureData.resize(poolSize);

		GL_ASSERT(glGenBuffers(1, &m_SSBO));
		GL_ASSERT(glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_SSBO));
		GL_ASSERT(glBufferData(GL_SHADER_STORAGE_BUFFER, m_ParticlePoolSize * sizeof(Particle), NULL, GL_STATIC_DRAW));
		GL_ASSERT(glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_SSBO));
	}

	void ParticleSystem2D::addParticle(const ParticleProperties& particleProperties, Particle& particle)
	{
		particle.Position = particleProperties.Position;
		particle.Rotation = Random::Float() * 360.0f;
		particle.RotationSpeed = Random::Float() * particleProperties.RotationSpeed;

		particle.Velocity = particleProperties.Velocity;
		particle.Velocity.x += particleProperties.VelocityVariation.x * (Random::Float() - 0.5f);
		particle.Velocity.y += particleProperties.VelocityVariation.y * (Random::Float() - 0.5f);

		particle.ColorBegin = particleProperties.ColorBegin;
		particle.ColorEnd = particleProperties.ColorEnd;

		particle.SizeBegin = particleProperties.SizeBegin + particleProperties.SizeVariation * (Random::Float() - 0.5f);
		particle.SizeEnd = particleProperties.SizeEnd;

		particle.LifeTime = particleProperties.LifeTime;
		particle.LifeRemaining = particleProperties.LifeTime;

		particle.Active = true;
	}

	template<>
	void ParticleSystem2D::onUpdate<UpdateDevice::CPU>(Timestep ts)
	{
		for (size_t i = 0; i < m_PoolIndex; ++i)
		{
			m_ParticlePool[i].LifeRemaining -= static_cast<float>(ts);
			m_ParticlePool[i].Position += m_ParticlePool[i].Velocity * static_cast<float>(ts);
			m_ParticlePool[i].Rotation += m_ParticlePool[i].RotationSpeed * static_cast<float>(ts);

			// Fade away particles
			float lifeRatio = m_ParticlePool[i].LifeRemaining / m_ParticlePool[i].LifeTime;
			m_ParticlePool[i].Color = glm::lerp(m_ParticlePool[i].ColorEnd, m_ParticlePool[i].ColorBegin, lifeRatio);
			m_ParticlePool[i].Color.a = lifeRatio;

			m_ParticlePool[i].Size = glm::lerp(m_ParticlePool[i].SizeEnd, m_ParticlePool[i].SizeBegin, lifeRatio);
		}
	}

	template<>
	void ParticleSystem2D::onUpdate<UpdateDevice::GPU>(Timestep ts)
	{
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_SSBO);
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, m_PoolIndex * sizeof(Particle), m_ParticlePool.data());

		m_ComputeShader.bind();
		m_ComputeShader.setUniform1f("u_Timestep", static_cast<float>(ts));
		glDispatchCompute((m_PoolIndex / m_WorkGroupSize) + 1, 1, 1);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

		glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, m_PoolIndex * sizeof(Particle), m_ParticlePool.data());
	}

	void ParticleSystem2D::Emit(const ParticleProperties& particleProperties)
	{
		Particle& particle = m_ParticlePool[m_PoolIndex];

		if (!particle.Active && m_PoolIndex < m_ParticlePoolSize)
		{
			addParticle(particleProperties, particle);
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
			if (m_ParticlePool[i].LifeRemaining <= 0.0f)
			{
				m_ParticlePool[i].Active = false;

				--m_PoolIndex;
				std::swap(m_ParticlePool[i], m_ParticlePool[m_PoolIndex]);
				std::swap(m_ParticleTextureData[i], m_ParticleTextureData[m_PoolIndex]);
				continue;
			}

			TextureData particleTextureData = m_ParticleTextureData[i];
			particleTextureData.RendererID = m_ParticleTextureData[i].RendererID > 1 ? m_ParticleTextureData[i].RendererID : 1;


			Renderer::drawQuadWithRotation(
				m_ParticlePool[i].Position,
				{ m_ParticlePool[i].Size, m_ParticlePool[i].Size },
				glm::radians(m_ParticlePool[i].Rotation),
				particleTextureData,
				{ 1.0f, 1.0f },
				m_ParticlePool[i].Color
			);
		}
		Renderer::endScene();
	}
}