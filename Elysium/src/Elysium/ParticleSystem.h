#pragma once

#include <functional>
#include <memory>
#include <vector>

#include <glm/glm.hpp>

#include "Elysium/Math.h"
#include "Elysium/Timestep.h"

#include "Elysium/Renderer/Camera.h"
#include "Elysium/Renderer/Renderer.h"
#include "Elysium/Renderer/Shader.h"

namespace Elysium
{
	enum class UpdateDevice
	{
		CPU = 0,
		GPU = 1
	};

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
			Vector4 ColorBegin, ColorEnd, Color;

			Vector2 Position;
			Vector2 Velocity;

			float Rotation;
			float RotationSpeed;

			float SizeBegin, SizeEnd, Size;

			float LifeTime;
			float LifeRemaining;

			bool Active = false;
		};

		bool m_Warning = false;
		uint32_t m_ParticlePoolSize;
		uint32_t m_PoolIndex;

		std::vector<Particle> m_ParticlePool;
		std::vector<TextureData> m_ParticleTextureData;

		unsigned int m_SSBO;
		Shader m_ComputeShader;

		int m_WorkGroupSize;

		using OnUpdateFunction = std::function<void(Timestep)>;

		OnUpdateFunction m_OnUpdate;

	private:
		void addParticle(const ParticleProperties& particleProperties, Particle& particle);

		void onCPUUpdate(Timestep ts);
		void onGPUUpdate(Timestep ts);

	public:
		// Initial ParticlePoolSize
		// Which is adjusted to a larger value in respect to the number of particles the user wants to emit
		ParticleSystem(uint32_t poolSize, UpdateDevice device = UpdateDevice::CPU);
		~ParticleSystem();

		// Emit a new particle
		void Emit(const ParticleProperties& particleProperties);

		void onUpdate(Timestep ts);
		void onRender(const OrthographicCamera& camera);
	};
}
