#pragma once

#include "Elysium/Timestep.h"

#include "Elysium/Renderer/Texture.h"

namespace Elysium
{
    template <size_t S>
    class Animation
    {
    private:
        float m_UpdateTime = 0.0f;
        float m_Runtime = 0.0f;
        size_t m_CurrentTexture = 0;

    public:
        TextureData Textures[S];

    public:
        Animation(float frameRate) : m_UpdateTime(1.0f / frameRate)
        {
        }

        inline TextureData getCurrentTexture() { return Textures[m_CurrentTexture]; }
        inline void reset() { m_CurrentTexture = 0; }
        inline void setAnimationFrameRate(float frameRate) { m_UpdateTime = 1.0f / frameRate; }

        void reflectAroundYAxis()
        {
            for (size_t i = 0; i < S; i++)
                Textures[i].reflectAroundYAxis();
        }

        void updateAnimation(Timestep ts)
        {
            m_Runtime += ts;
            if (m_Runtime >= m_UpdateTime)
            {
                m_CurrentTexture = ++m_CurrentTexture % S;
                m_Runtime = 0.0f;
            }
        }
    };
}
