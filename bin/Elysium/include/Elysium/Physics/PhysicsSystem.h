#pragma once

#include <unordered_set>

#include "Elysium/Math.h"
#include "Elysium/Physics/PhysicalBody.h"

namespace Elysium
{
    using BodyHandle = uint32_t;

    class PhysicsSystem final
    {
    private:
        float m_Time = 0.0f;
        float m_GravitationalAccel;

        ArrayList<PhysicalBody> m_Bodies;
        std::vector<BodyHandle> m_InactiveBodies;

        std::unordered_set<size_t> m_LoggedBodies;

    public:
        bool Gravity = true;

    private:
        void updateBody(PhysicalBody& body, Timestep ts);
        void applyCollisionResponse(PhysicalBody& body, const PhysicalBody& otherBody, const Vector2& normal, float overlap,
            Timestep ts);
        bool checkBroadPhase(const PhysicalBody& body1, const PhysicalBody& body2);
        void checkNarrowPhase(const PhysicalBody& body1, const PhysicalBody& body2, CollisionInfo& info);

        void printInfo(BodyHandle i);

    public:
        PhysicsSystem(float acceleration);
        ~PhysicsSystem();

        PhysicalBody* createPhysicalBody(BodyType type, const char* name, float mass, const Vector2& initialPosition, const Vector2& size,
            PhysicalBody::Collision_Callback callback = nullptr);

        void createPhysicalBody(BodyHandle* handle, BodyType type, const char* name, float mass, const Vector2& initialPosition, const Vector2& size,
            PhysicalBody::Collision_Callback callback = nullptr);

        inline float getTime() { return m_Time; }
        inline void setGravitaionnalAccel(float acceleration) { m_GravitationalAccel = acceleration; }
        inline float getGravitaionnalAccel() const { return m_GravitationalAccel; }

        inline ArrayList<PhysicalBody>& getBodies() { return m_Bodies; }
        inline void clear()
        {
            m_Bodies.clear();
            m_InactiveBodies.clear();
            m_LoggedBodies.clear();
        }

        inline PhysicalBody* getPhysicalBody(BodyHandle identifier) { return &m_Bodies[identifier]; };
        inline const PhysicalBody& readPhysicalBody(BodyHandle identifier) const { return m_Bodies[identifier]; };
        void removePhysicalBody(BodyHandle body);

        void logInfo(const char* tag);

        void onUpdate(Timestep ts);
    };
}