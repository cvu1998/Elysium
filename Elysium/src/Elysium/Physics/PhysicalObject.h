#pragma once

#include <glm/glm.hpp>

#include "Elysium/Timestep.h"
#include "Elysium/Renderer/Renderer2D.h"

enum class ObjectType
{
    STATIC = 0,
    DYNAMIC = 1
};

namespace Elysium
{
    class PhysicalObject
    {
    protected:
        ObjectType m_Type;

        glm::vec2 Position = { 0.0f, 0.0f };

    public:
        glm::vec2 Velocity = { 0.0f, 0.0f };
        glm::vec2 Acceleration = { 0.0f, 0.0f };
        glm::vec2 LastAcceleration = { 0.0f, 0.0f };

        glm::vec2 Size = { 1.0f, 1.0f };
        float Rotation = 0.0f;

        glm::vec4 Color = { 1.0f, 1.0f, 1.0f, 1.0f };
        unsigned int TextureID = 1;

    public:
        PhysicalObject(glm::vec2& initialPosition, ObjectType type);

        inline virtual ObjectType getType() const final { return m_Type; }
        inline virtual glm::vec2 getPosition() const final { return Position; }

        virtual bool isBoxColliding(const PhysicalObject* object) const final;

        virtual glm::vec2 getFuturePosition(Timestep ts) const = 0;
        virtual void onCollision() = 0;
        virtual void onUpdate(Timestep ts) = 0;
    };
}