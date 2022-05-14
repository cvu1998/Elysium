#pragma once

#include <functional>
#include <limits>

#include "Elysium/Math.h"
#include "Elysium/Timestep.h"

namespace Elysium
{
    enum class BodyType
    {
        NONE = -1,
        STATIC = 0,
        KINEMATIC = 1,
        DYNAMIC = 2
    };

    enum class BodyStatus
    {
        INACTIVE = 0,
        ACTIVE = 1
    };

    enum class Collider
    {
        QUAD = 1,
        CIRCLE =  2
    };

    struct BodyCollisionInfo
    {
        Vector2 Normal = { 0.0f, 0.0f };
    };

    struct CollisionInfo
    {
        bool Collision = false;
        float minOverlap = std::numeric_limits<float>::max();
        std::pair<BodyCollisionInfo, BodyCollisionInfo> CollisionInfoPair;

        Timestep ts;
    };

    class PhysiscsSystem2D;

    class PhysicalBody2D
    {
        friend class PhysicsSystem2D;

        using Collision_Callback = std::function<void(PhysicalBody2D& body,
            PhysicalBody2D& collidee,
            const CollisionInfo& collisionInfo)>;

    private:
        BodyType Type = BodyType::NONE;
        BodyStatus Status = BodyStatus::ACTIVE;
        Collider Model = Collider::QUAD;

        const char* Tag = nullptr;
        float Mass = std::numeric_limits<float>::max();
        float AngularVelocity = 0.0f;
        float Inertia = 0.0f;
        Vector2 Size = { 1.0f, 1.0f };

        Vector2 MaximumVertex = { 0.0f, 0.0f };
        Vector2 MinimumVertex = { 0.0f, 0.0f };

        Vector2 Normal = { 0.0f, 0.0f };

        Vector2 ContactImpulse = { 0.0f, 0.0f };
        Vector2 ContactNormal = { 0.0f, 0.0f };
        
        float ElasticityCoefficient = 1.0f;
        float FrictionCoefficient = 0.0f;
        float GravitationalAccel = 0.0f;

        std::vector<Vector2> m_ModelVertices;
        std::vector<Vector2> m_Normals;
        std::vector<Vector2> Normals;

        Collision_Callback Callback = nullptr;
        unsigned int NumberOfExecution = 0;

    public:
        Vector2 Position = { 0.0f, 0.0f };
        Vector2 Velocity = { 0.0f, 0.0f };
        Vector2 Acceleration = { 0.0f, 0.0f };

        float Rotation = 0.0f;
        bool AllowRotation = true;

        Vector2 Force = { 0.0f, 0.0f };
        Vector2 Impulse = { 0.0f, 0.0f };

        unsigned int CallbackExecutions = 0;

    private:
        PhysicalBody2D() = default;
        PhysicalBody2D(BodyType type, Collider collider, const char* tag, float mass,
            const Vector2& initialPosition, const Vector2& size,
            Collision_Callback callback);

        Vector2 tranformVertex(const Vector2& vertex) const;

        void getVertices(std::vector<Vector2>& vertices) const;

        Vector2 getMaxVertex() const;
        Vector2 getMinVertex() const;

        void updateNormals();

        void onUpdate(Timestep ts);

    public:
        inline const char* getTag() const { return Tag; }
        inline float getMass() const { return Mass; }
        inline const Vector2& getSize() const { return Size; }

        void setElasticityCoefficient(float coefficient);
        void setFrictionCoefficient(float coefficient);
        inline float getElasticityCoefficient() const { return ElasticityCoefficient; }
        inline float getFrictionCoefficient() const { return FrictionCoefficient; }

        inline void setNumberOfCallbackExecution(unsigned int number) { NumberOfExecution = number; }

        inline void setModelVertices(const std::vector<Vector2>& vertices) { m_ModelVertices = vertices; }

        void resetValues();
    };
}