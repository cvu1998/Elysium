#pragma once

#include <glm/glm.hpp>

#include "Elysium/Timestep.h"
#include "Elysium/Renderer/Renderer2D.h"

enum class ObjectType
{
    STATIC = 0,
    DYNAMIC = 1
};

enum class CollisionOccurence
{
    TOP = 0,
    BOTTOM,
    LEFT,
    RIGHT
};

namespace Elysium
{
    class PhysicalObject
    {
    protected:
        ObjectType m_Type;

        glm::vec2 Position;

        glm::vec2 LastAcceleration = { 0.0f, 0.0f };
        
        float ElasticityCoefficient = 0.0f;
        float FrictionCoefficient = 0.0f;
        float GravitationalAccel = 0.0f;

    public:
        glm::vec2 Velocity = { 0.0f, 0.0f };
        glm::vec2 Acceleration = { 0.0f, 0.0f };

        glm::vec2 Size = { 1.0f, 1.0f };
        float Rotation = 0.0f;

        glm::vec4 Color = { 1.0f, 1.0f, 1.0f, 1.0f };
        unsigned int TextureID = 1;

    public:
        PhysicalObject(glm::vec2& initialPosition, ObjectType type);

        inline virtual ObjectType getType() const final { return m_Type; }
        inline virtual glm::vec2 getPosition() const final { return Position; }
        inline virtual glm::vec2 getLastAcceleration() const final { return LastAcceleration; }
        inline virtual float getElasticityCoefficient() const final { return ElasticityCoefficient; }
        inline virtual float getFrictionCoefficient() const final { return FrictionCoefficient; }

        virtual void setElasticityCoefficient(float coefficient) final;
        virtual void setFrictionCoefficient(float coefficient) final;
        inline virtual void setGravitationalAccel(float acceleration) final { GravitationalAccel = acceleration; }

        virtual CollisionOccurence getCollisionOccurence(const PhysicalObject* object) const final;

        virtual glm::vec2 getFuturePosition(Timestep ts) const = 0;
        virtual void onCollision() = 0;
        virtual void onUpdate(Timestep ts) = 0;
    };
}