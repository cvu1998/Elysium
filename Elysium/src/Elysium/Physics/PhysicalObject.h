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
        friend class PhysicsSystem;

    protected:
        ObjectType m_Type;
        unsigned int m_ID = -1;

        glm::vec2 Position;
        glm::vec2 Size = { 1.0f, 1.0f };
        std::array<glm::vec2, 4> VerticesPosition;
        glm::vec2 Velocity = { 0.0f, 0.0f };
        glm::vec2 Acceleration = { 0.0f, 0.0f };

        float Rotation = 0.0f;
        
        float ElasticityCoefficient = 1.0f;
        float FrictionCoefficient = 0.0f;
        float GravitationalAccel = 0.0f;

    public:
        float Mass = 1.0f;
        glm::vec2 Force = { 0.0f, 0.0f };
        glm::vec2 Impulse = { 0.0f, 0.0f };

        glm::vec4 Color = { 1.0f, 1.0f, 1.0f, 1.0f };
        unsigned int TextureID = 1;

    public:
        PhysicalObject(const glm::vec2& initialPosition, const glm::vec2& size, ObjectType type);

        inline virtual ObjectType getType() const final { return m_Type; }
        inline virtual unsigned int getSystemID() const final { return m_ID; }

        inline virtual const glm::vec2& getPosition() const final { return Position; }
        inline virtual const glm::vec2& getVelocity() const final { return Velocity; }
        inline virtual const glm::vec2& getAcceleration() const final { return Acceleration; }
        inline virtual float getMass() const final { return Mass; }
        inline virtual float getRotation() const final { return Rotation; }
        inline virtual float getElasticityCoefficient() const final { return ElasticityCoefficient; }
        inline virtual float getFrictionCoefficient() const final { return FrictionCoefficient; }
        inline virtual const std::array<glm::vec2, 4>& getVerticesPosition() const final { return VerticesPosition; }

        virtual void setElasticityCoefficient(float coefficient) final;
        virtual void setFrictionCoefficient(float coefficient) final;
        inline virtual void setGravitationalAccel(float acceleration) final { GravitationalAccel = acceleration; }

        virtual void Draw() final;

        virtual CollisionOccurence getCollisionOccurence(const PhysicalObject* object) const final;

        virtual glm::vec2 getFuturePosition(const glm::vec2& position, Timestep ts) const = 0;
        virtual void onCollision() = 0;
        virtual void onUpdate(Timestep ts) = 0;
    };
}