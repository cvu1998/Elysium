#pragma once

#include <limits>

#include "Elysium/ArrayList.h"
#include "Elysium/Math.h"
#include "Elysium/Timestep.h"
#include "Elysium/Renderer/Renderer2D.h"

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

    struct BodyCollisionInfo
    {
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
        friend class ArrayList<PhysicalBody>;
        friend class PhysicsSystem;

        using Collision_Callback = void (*)(PhysicalBody& body,
            PhysicalBody& collidee,
            const CollisionInfo& collisionInfo);

    private:
        const char* Tag = nullptr;
        float Mass = 0.0f;
        float Inertia = 0.0f;
        float Radius = 0.0f;
        Vector2 Size = { 1.0f, 1.0f };

        Vector2 MaximumVertex = { 0.0f, 0.0f };
        Vector2 MinimumVertex = { 0.0f, 0.0f };

        struct Hash_Vector2
        {
            size_t operator()(const Vector2& vector) const
            {
                auto hash1 = std::hash<float>{}(vector.x);
                auto hash2 = std::hash<float>{}(vector.y);
                return hash1 ^ hash2;
            }
        };

        std::unordered_map<Vector2, Vector2, Hash_Vector2> Normals;
        Vector2 ContactNormal = { 0.0f, 0.0f };

        std::vector<Vector2> m_ModelVertices;
        
        float ElasticityCoefficient = 1.0f;
        float FrictionCoefficient = 1.0f;
        float GravitationalAccel = 0.0f;

        Collision_Callback Callback = nullptr;
        unsigned int NumberOfExecution = 0;

    public:
        Vector2 Position = { 0.0f, 0.0f };
        Vector2 Velocity = { 0.0f, 0.0f };
        Vector2 Acceleration = { 0.0f, 0.0f };

        float Rotation = 0.0f;
        bool AllowRotation = true;

        BodyType Type = BodyType::NONE;
        BodyStatus Status = BodyStatus::ACTIVE;

        Vector2 Force = { 0.0f, 0.0f };
        Vector2 Impulse = { 0.0f, 0.0f };

        unsigned int CallbackExecutions = 0;

    private:
        PhysicalBody();
        PhysicalBody(BodyType type, const char* tag, float mass, const Vector2& initialPosition, const Vector2& size,
            Collision_Callback callback);

        Vector2 tranformVertex(const Vector2& vertex) const;

    public:
        static PhysicalBody createPhysicalBody(BodyType type, const char* tag, float mass, const Vector2& initialPosition, const Vector2& size,
            Collision_Callback callback);

        inline const char* getTag() const { return Tag; }
        inline float getMass() const { return Mass; }
        inline const Vector2& getSize() const { return Size; }

        void setRadius(float radius);
        void setElasticityCoefficient(float coefficient);
        void setFrictionCoefficient(float coefficient);
        inline float getRadius() const { return Radius; }
        inline float getElasticityCoefficient() const { return ElasticityCoefficient; }
        inline float getFrictionCoefficient() const { return FrictionCoefficient; }

        inline void setNumberOfCallbackExecution(unsigned int number) { NumberOfExecution = number; }

        void setModelVertices(const std::vector<Vector2>& vertices) { m_ModelVertices = vertices; }
        const std::vector<Vector2>& getModelVertices() const { return m_ModelVertices; }
        std::vector<Vector2>getVertices() const;

        Vector2 getMaxVertex() const;
        Vector2 getMinVertex() const;

        std::vector<Vector2>getNormals() const;

        void resetValues();
    };
}