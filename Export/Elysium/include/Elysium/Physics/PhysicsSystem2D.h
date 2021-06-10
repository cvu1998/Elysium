#pragma once

#include <deque>
#include <unordered_set>

#include "Elysium/Math.h"
#include "Elysium/Physics/PhysicalBody2D.h"

namespace Elysium
{
    using BodyHandle = uint32_t;

    class PhysicsSystem2D final
    {
    private:
        float m_Time = 0.0f;

        std::deque<PhysicalBody2D> m_Bodies;
        std::vector<BodyHandle> m_InactiveBodies;

        std::unordered_set<size_t> m_LoggedBodies;

    public:
        bool Gravity = true;
        float GravitationalAccel = -9.80f;

    private:
        bool checkBroadPhase(const PhysicalBody2D& body1, const PhysicalBody2D& body2);
        void checkNarrowPhase(const PhysicalBody2D& body1, const PhysicalBody2D& body2, CollisionInfo& info);

        void applyCollisionResponse(PhysicalBody2D& body, const PhysicalBody2D& otherBody, const Vector2& normal, float overlap,
            Timestep ts);

        void printInfo(BodyHandle i);

    public:
        PhysicsSystem2D() = default;

        PhysicalBody2D* createPhysicalBody(BodyType type, Collider collider, const char* name, float mass,
            const Vector2& initialPosition, const Vector2& size,
            PhysicalBody2D::Collision_Callback callback = nullptr);

        void createPhysicalBody(BodyHandle* handle, BodyType type, Collider collider, const char* name, float mass,
            const Vector2& initialPosition, const Vector2& size,
            PhysicalBody2D::Collision_Callback callback = nullptr);

        void removePhysicalBody(BodyHandle body);

        inline PhysicalBody2D* getPhysicalBody(BodyHandle identifier) { return &m_Bodies[identifier]; };
        inline const PhysicalBody2D& readPhysicalBody(BodyHandle identifier) const { return m_Bodies[identifier]; };

        inline float getTime() { return m_Time; }

        inline std::deque<PhysicalBody2D>& getBodies() { return m_Bodies; }
        inline void clear()
        {
            m_Bodies.clear();
            m_InactiveBodies.clear();
            m_LoggedBodies.clear();
        }

        void logInfo(const char* tag);

        void onUpdate(Timestep ts);
    };
}