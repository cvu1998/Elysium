#pragma once

#include <functional>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

#include <glm/glm.hpp>

#include "Elysium/Math.h"
#include "Elysium/Timestep.h"
#include "Elysium/Utility.h"

#include "Elysium/Renderer/Camera.h"
#include "Elysium/Renderer/Renderer.h"
#include "Elysium/Renderer/Shader.h"

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

	class ParticleSystem2D
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
		size_t m_ParticlePoolSize;
		uint32_t m_PoolIndex;

		std::vector<Particle> m_ParticlePool;
		std::vector<TextureData> m_ParticleTextureData;

		unsigned int m_SSBO;
		Shader m_ComputeShader;

		int m_WorkGroupSize;

	private:
		ParticleSystem2D(size_t poolSize);
		~ParticleSystem2D();

		ParticleSystem2D(const ParticleSystem2D&) = delete;
		ParticleSystem2D(ParticleSystem2D&&) = delete;
		ParticleSystem2D& operator=(const ParticleSystem2D&) = delete;
		ParticleSystem2D& operator=(ParticleSystem2D&&) = delete;

		void addParticle(const ParticleProperties& particleProperties, Particle& particle);

	public:
		static void Init(size_t poolSize);
		static void Shutdown();

		static ParticleSystem2D& Get();

		// Emit a new particle
		void Emit(const ParticleProperties& particleProperties);
		void reset(size_t poolSize);
		void resizePool(size_t poolSize);

		template<UpdateDevice T>
		void onUpdate(Timestep ts);

		template<>
		void onUpdate<UpdateDevice::CPU>(Timestep ts);

		template<>
		void onUpdate<UpdateDevice::GPU>(Timestep ts);

		void onRender(const OrthographicCamera& camera);
	};
}
