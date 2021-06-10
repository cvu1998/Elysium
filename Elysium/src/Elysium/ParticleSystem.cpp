#include "ParticleSystem.h"

#include <glm/gtc/constants.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/compatibility.hpp>

#include "Elysium/Random.h"

#include "Elysium/Log.h"

namespace Elysium
{
	ParticleSystem::ParticleSystem(uint32_t poolSize, UpdateDevice device) :
		m_ParticlePoolSize(poolSize),
		m_PoolIndex(0),
		m_ComputeShader("res/shaders/particles.glsl", ShaderType::COMPUTE)
	{
		m_ParticlePool.resize(poolSize);
		m_ParticleTextureData.resize(poolSize);

		switch (device)
		{
		case UpdateDevice::CPU:
			m_OnUpdate = std::bind(&ParticleSystem::onCPUUpdate, this, std::placeholders::_1);
			break;
		case UpdateDevice::GPU:
			m_OnUpdate = std::bind(&ParticleSystem::onGPUUpdate, this, std::placeholders::_1);
			glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &m_WorkGroupSize);
#ifdef _DEBUG
			ELY_CORE_INFO("Work Group Size: {0}", m_WorkGroupSize);
#endif

			GL_ASSERT(glGenBuffers(1, &m_SSBO));
			GL_ASSERT(glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_SSBO));
			GL_ASSERT(glBufferData(GL_SHADER_STORAGE_BUFFER, m_ParticlePoolSize * sizeof(Particle), NULL, GL_STATIC_DRAW));
			GL_ASSERT(glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_SSBO));
			break;
		}

	}

	ParticleSystem::~ParticleSystem()
	{
		GL_ASSERT(glDeleteBuffers(1, &m_SSBO));
	}

	void ParticleSystem::addParticle(const ParticleProperties& particleProperties, Particle& particle)
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

	void ParticleSystem::onCPUUpdate(Timestep ts)
	{
		std::for_each(m_ParticlePool.begin(), m_ParticlePool.begin() + m_PoolIndex, [ts](Particle& particle)
			{
				particle.LifeRemaining -= ts;
				particle.Position += particle.Velocity * (float)ts;
				particle.Rotation += particle.RotationSpeed * ts;

				// Fade away particles
				float lifeRatio = particle.LifeRemaining / particle.LifeTime;
				particle.Color = glm::lerp(particle.ColorEnd, particle.ColorBegin, lifeRatio);
				particle.Color.a = lifeRatio;

				particle.Size = glm::lerp(particle.SizeEnd, particle.SizeBegin, lifeRatio);
			});
	}

	void ParticleSystem::onGPUUpdate(Timestep ts)
	{
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_SSBO);
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, m_PoolIndex * sizeof(Particle), m_ParticlePool.data());

		m_ComputeShader.bind();
		m_ComputeShader.setUniform1f("u_Timestep", (float)ts);
		glDispatchCompute((m_PoolIndex / m_WorkGroupSize) + 1, 1, 1);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

		glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, m_PoolIndex * sizeof(Particle), m_ParticlePool.data());
	}

	void ParticleSystem::Emit(const ParticleProperties& particleProperties)
	{
		Particle& particle = m_ParticlePool[m_PoolIndex];

		if (!particle.Active)
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

	void ParticleSystem::onUpdate(Timestep ts)
	{
		m_OnUpdate(ts);
	}

	void ParticleSystem::onRender(const OrthographicCamera& camera)
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


			Renderer::drawQuadWithRotation(m_ParticlePool[i].Position, { m_ParticlePool[i].Size, m_ParticlePool[i].Size }, glm::radians(m_ParticlePool[i].Rotation), particleTextureData,
				{ 1.0f, 1.0f },
				m_ParticlePool[i].Color);
		}
		Renderer::endScene();
	}
}