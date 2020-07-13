#pragma once

#include "Elysium/Math.h"
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
        friend class PhysicsSystem;

    protected:
        ObjectType m_Type;
        
        const char* Name;
        Vector2 Position = {0.0f, 0.0f};
        Vector2 Size = { 1.0f, 1.0f };
        float BroadSize = 0.0f;
        Vector2 Velocity = { 0.0f, 0.0f };
        Vector2 Acceleration = { 0.0f, 0.0f };

        float Rotation = 0.0f;

        std::vector<Vector2> m_ModelVertices;
        
        float ElasticityCoefficient = 1.0f;
        float FrictionCoefficient = 0.0f;
        float GravitationalAccel = 0.0f;

    public:
        float Mass = 1.0f;
        Vector2 Force = { 0.0f, 0.0f };
        Vector2 Impulse = { 0.0f, 0.0f };

        Vector4 Color = { 1.0f, 1.0f, 1.0f, 1.0f };
        TextureData TextureData;

    protected:
        virtual Vector2 tranformVertex(const Vector2& vertex) final;

    public:
        PhysicalObject(const char* name, const Vector2& initialPosition, const Vector2& size, ObjectType type);

        inline virtual ObjectType getType() const final { return m_Type; }

        inline virtual const Vector2& getPosition() const final { return Position; }
        inline virtual const Vector2& getVelocity() const final { return Velocity; }
        inline virtual const Vector2& getAcceleration() const final { return Acceleration; }
        inline virtual const Vector2 getSize() const final { return Size; }
        inline virtual float getRotation() const final { return Rotation; };

        virtual void setElasticityCoefficient(float coefficient) final;
        virtual void setFrictionCoefficient(float coefficient) final;
        inline virtual float getElasticityCoefficient() const final { return ElasticityCoefficient; }
        inline virtual float getFrictionCoefficient() const final { return FrictionCoefficient; }

        virtual void setModelVertices(const std::vector<Vector2>& vertices) final { m_ModelVertices = vertices; }
        virtual std::vector<Vector2>getModelVertices() final;

        virtual std::vector<Vector2>getNormals();
        virtual void Draw();

        virtual void onCollision() = 0;
        virtual void onUpdate(Timestep ts) = 0;
    };

    struct ObjectCollisionInfo
    {
        PhysicalObject* object = nullptr;
        Vector2 Normal = { 0.0f, 0.0f };
        Vector2 MTV = { 0.0f, 0.0f };
    };
}