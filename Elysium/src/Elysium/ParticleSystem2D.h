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
#include "Elysium/Timer.h"

#include "Elysium/Renderer/Camera.h"
#include "Elysium/Renderer/Renderer.h"
#include "Elysium/Renderer/Shader.h"

namespace Elysium
{
	struct ParticleProperties
	{
		Vector2		Position			= { 0.0f, 0.0f };
		Vector2		Velocity			= { 0.0f, 10.0f };
		Vector2		VelocityVariation	= { 1.0f, 1.0f };
		float		RotationSpeed		= 180.f;

		Vector4		ColorBegin	= { 1.0f, 1.0f, 1.0f, 1.0f };
		Vector4		ColorEnd	= { 0.0f, 0.0f, 0.0f, 1.0f };

		TextureData	TextureData;

		float		SizeBegin		= 1.0f;
		float		SizeEnd			= 0.0f;
		float		SizeVariation	= 0.1f;

		float		LifeTime = 1.0f;
	};

	class ParticleSystem2D
	{
	private:
		bool m_Warning = false;
		size_t m_ParticlePoolSize;
		uint32_t m_PoolIndex;

		std::vector<Vector2> m_Position;
		std::vector<Vector2> m_Velocity;

		std::vector<float> m_Rotation;
		std::vector<float> m_RotationSpeed;

		std::vector<float> m_SizeBegin;
		std::vector<float> m_SizeEnd;
		std::vector<float> m_Size;

		std::vector<Vector4> m_ColorBegin;
		std::vector<Vector4> m_ColorEnd;
		std::vector<Vector4> m_Color;

		std::vector<float> m_LifeTime;
		std::vector<float> m_LifeRemaining;

		std::vector<TextureData> m_ParticleTextureData;

		static constexpr int s_BufferCount = 12;
		std::array<unsigned int, s_BufferCount> m_SSBO;
		Shader m_ComputeShader;

		int m_WorkGroupSize;

	private:
		ParticleSystem2D(size_t poolSize);
		~ParticleSystem2D();

		ParticleSystem2D(const ParticleSystem2D&) = delete;
		ParticleSystem2D(ParticleSystem2D&&) = delete;
		ParticleSystem2D& operator=(const ParticleSystem2D&) = delete;
		ParticleSystem2D& operator=(ParticleSystem2D&&) = delete;

		void addParticle(const ParticleProperties& particleProperties, size_t index);

		template<typename f>
		void applyOnVectors(f&& function)
		{
			function(m_Position);
			function(m_Velocity);

			function(m_Rotation);
			function(m_RotationSpeed);

			function(m_SizeBegin);
			function(m_SizeEnd);
			function(m_Size);

			function(m_ColorBegin);
			function(m_ColorEnd);
			function(m_Color);

			function(m_LifeTime);
			function(m_LifeRemaining);

			function(m_ParticleTextureData);
		}

	public:
		static void Init(size_t poolSize);
		static void Shutdown();

		static ParticleSystem2D& Get();

		// Emit a new particle
		void Emit(const ParticleProperties& particleProperties);
		void reset(size_t poolSize);
		void resizePool(size_t poolSize);

		template<UpdateDevice D>
		void onUpdate(Timestep ts);

		template<>
		void onUpdate<UpdateDevice::CPU>(Timestep ts);

		template<>
		void onUpdate<UpdateDevice::GPU>(Timestep ts);

		template<UpdateDevice D>
		void onRender(const OrthographicCamera& camera);

		template<>
		void onRender<UpdateDevice::CPU>(const OrthographicCamera& camera);

		template<>
		void onRender<UpdateDevice::GPU>(const OrthographicCamera& camera);
	};
}
