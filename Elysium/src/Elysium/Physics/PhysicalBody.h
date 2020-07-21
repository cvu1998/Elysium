#pragma once

#include "Elysium/Math.h"
#include "Elysium/Timestep.h"
#include "Elysium/Renderer/Renderer2D.h"

namespace Elysium
{
    enum class BodyType
    {
        STATIC = 0,
        DYNAMIC = 1
    };

    enum class BodyStatus
    {
        INACTIVE = 0,
        ACTIVE = 1,
        NOGRAVITY = 2
    };

    struct BodyCollisionInfo
    {
        const char* BodyName;
        Vector2 Normal = { 0.0f, 0.0f };
    };

    struct CollisionInfo
    {
        bool Collision = true;
        float minOverlap = std::numeric_limits<float>::max();
        Timestep ts;
        std::pair<BodyCollisionInfo, BodyCollisionInfo> CollisionInfoPair;
    };

    class PhysiscsSystem;

    class PhysicalBody
    {
        friend class PhysicsSystem;

        typedef void (*Collision_Callback) (PhysicalBody&, const CollisionInfo&);

    private:      
        BodyType Type;
        const char* Name;
        float Mass = 0.0f;
        Vector2 Velocity = { 0.0f, 0.0f };
        Vector2 Acceleration = { 0.0f, 0.0f };

        float BroadSize = 0.0f;
        std::vector<Vector2> m_ModelVertices;
        
        float ElasticityCoefficient = 1.0f;
        float FrictionCoefficient = 1.0f;
        float GravitationalAccel = 0.0f;

        Collision_Callback Callback;

    public:
        Vector2 Position = { 0.0f, 0.0f };
        Vector2 Size = { 1.0f, 1.0f };
        float Radius = 0.0f;
        float Rotation = 0.0f;

        BodyStatus Status = BodyStatus::ACTIVE;

        Vector2 Force = { 0.0f, 0.0f };
        Vector2 Impulse = { 0.0f, 0.0f };

    private:
        Vector2 tranformVertex(const Vector2& vertex) const;

    public:
        PhysicalBody(BodyType type, const char* name, float mass, const Vector2& initialPosition, const Vector2& size,
            Collision_Callback callback);

        inline const char* getName() const { return Name; }
        inline float getMass() const { return Mass; }
        inline const Vector2& getVelocity() const { return Velocity; }
        inline const Vector2& getAcceleration() const { return Acceleration; }
        inline const Vector2& getSize() const { return Size; }
        inline float getRotation() const { return Rotation; };

        void setElasticityCoefficient(float coefficient);
        void setFrictionCoefficient(float coefficient);
        inline float getElasticityCoefficient() const { return ElasticityCoefficient; }
        inline float getFrictionCoefficient() const { return FrictionCoefficient; }

        void setModelVertices(const std::vector<Vector2>& vertices) { m_ModelVertices = vertices; }
        const std::vector<Vector2>& getModelVertices() const { return m_ModelVertices; }
        std::vector<Vector2>getVertices() const;

        std::vector<Vector2>getNormals() const;
    };
}