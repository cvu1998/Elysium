#include "ParticleSystem.h"

#include <glm/gtc/constants.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/compatibility.hpp>

#include "Random.h"

namespace Elysium
{
	ParticleSystem::ParticleSystem(uint32_t poolSize, OrthographicCamera& camera) : 
		m_InitialParticlePoolSize(poolSize), m_ParticlePoolSize(poolSize),
		m_PoolIndex(0),
		m_Camera(&camera)
	{
		m_ParticlePool.resize(poolSize);
	}

	ParticleSystem::~ParticleSystem()
	{
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
		particle.TextureData = particleProperties.TextureData;

		particle.SizeBegin = particleProperties.SizeBegin + particleProperties.SizeVariation * (Random::Float() - 0.5f);
		particle.SizeEnd = particleProperties.SizeEnd;

		particle.LifeTime = particleProperties.LifeTime;
		particle.LifeRemaining = particleProperties.LifeTime;
		particle.Active = true;
	}

	void ParticleSystem::Emit(const ParticleProperties& particleProperties)
	{
		Particle& particle = m_ParticlePool[m_PoolIndex];

		if (!particle.Active)
		{
			addParticle(particleProperties, particle);
		}
		else
		{
			m_PoolIndex = m_ParticlePoolSize;
			m_ParticlePoolSize += m_InitialParticlePoolSize * m_NumberOfResize;
			m_ParticlePool.resize(m_ParticlePoolSize);

			Particle& particle = m_ParticlePool[m_PoolIndex];

			addParticle(particleProperties, particle);

			m_NumberOfResize++;
		}
		m_PoolIndex = ++m_PoolIndex % m_ParticlePool.size();
	}

	void ParticleSystem::OnUpdate(Timestep ts)
	{
		for (auto& particle : m_ParticlePool)
		{
			if (!particle.Active)
				continue;

			if (particle.LifeRemaining <= 0.0f)
			{
				particle.Active = false;
				continue;
			}

			particle.LifeRemaining -= ts;
			particle.Position += particle.Velocity * (float)ts;
			particle.Rotation += particle.RotationSpeed * ts;
		}

		Renderer::beginScene(*m_Camera);
		for (auto& particle : m_ParticlePool)
		{
			if (!particle.Active)
				continue;

			// Fade away particles
			float lifeRatio = particle.LifeRemaining / particle.LifeTime;
			glm::vec4 color = glm::lerp(particle.ColorEnd, particle.ColorBegin, lifeRatio);
			color.a = color.a * lifeRatio;

			float size = glm::lerp(particle.SizeEnd, particle.SizeBegin, lifeRatio);

			if (!particle.TextureData.isDefault())
				Renderer::drawQuadWithRotation(particle.Position, { size, size }, glm::radians(particle.Rotation), particle.TextureData, color);
			else
				Renderer::drawQuadWithRotation(particle.Position, { size, size }, glm::radians(particle.Rotation), color);
		}
		Renderer::endScene();
	}
}