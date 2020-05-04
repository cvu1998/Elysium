#pragma once

#include <glm/glm.hpp>

#include "Elysium/Timestep.h"
#include "Elysium/Renderer/Renderer2D.h"

namespace Elysium
{
    class PhysicalObject
    {
    private:
        glm::vec2 m_Position = { 0.0f, 0.0f };
        glm::vec2 m_Veclocity = { 0.0f, 0.0f };

    public:
        glm::vec2 InitialPosition = { 0.0f, 0.0f };
        glm::vec2 Size = { 1.0f, 1.0f };
        glm::vec2 InitialVelocity = { 0.0f, 0.0f };
        glm::vec2 Acceleration = { 0.0f, 0.0f };
        float Rotation = 0.0f;

        glm::vec4 Color = { 1.0f, 1.0f, 1.0f, 1.0f };
        unsigned int TextureID = 1;

        void onUpdate(Timestep ts, float time, float acceleration);
    };
}