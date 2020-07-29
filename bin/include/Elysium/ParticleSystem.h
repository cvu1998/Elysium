#pragma once

#include <memory>
#include <vector>

#include <glm/glm.hpp>

#include "Elysium/Math.h"
#include "Elysium/Timestep.h"

#include "Elysium/Renderer/Camera.h"
#include "Elysium/Renderer/Renderer2D.h"

namespace Elysium
{
	struct ParticleProperties
	{
		Vector2		Position = { 0.0f, 0.0f };
		Vector2		Velocity = { 0.0f, 10.0f };
		Vector2		VelocityVariation = { 1.0f, 1.0f };
		float			RotationSpeed = 180.f;

		Vector4		ColorBegin = { 1.0f, 1.0f, 1.0f, 1.0f };
		Vector4		ColorEnd = { 0.0f, 0.0f, 0.0f, 1.0f };
		TextureData		TextureData;

		float			SizeBegin = 1.0f;
		float			SizeEnd = 0.0f;
		float			SizeVariation = 0.1f;

		float			LifeTime = 1.0f;
	};

	class ParticleSystem
	{
	private:
		struct Particle
		{
			Vector2 Position;
			Vector2 Velocity;
			float Rotation;
			float RotationSpeed;

			Vector4 ColorBegin, ColorEnd;
			TextureData TextureData;

			float SizeBegin, SizeEnd;

			float LifeTime;
			float LifeRemaining;
			bool Active = false;
		};

		uint32_t m_NumberOfResize = 1;
		uint32_t m_InitialParticlePoolSize;
		uint32_t m_ParticlePoolSize;
		uint32_t m_PoolIndex;
		std::vector<Particle> m_ParticlePool;

		OrthographicCamera* m_Camera;

	private:
		void addParticle(const ParticleProperties& particleProperties, Particle& particle);

	public:
		// Initial ParticlePoolSize
		// Which is resized to a larger value in respect to the number of particled the user wants to emit
		ParticleSystem(uint32_t poolSize, OrthographicCamera& camera);
		~ParticleSystem();

		// Emit a new particle
		void Emit(const ParticleProperties& particleProperties);

		void OnUpdate(Timestep ts);
	};
}
