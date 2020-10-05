#pragma once

#include "Elysium/Math.h"
#include "Elysium/Physics/PhysicalBody.h"

//#define LOG

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

    public:
        bool Gravity = true;

    private:
        void updateBody(PhysicalBody& body, Timestep ts);
        void applyCollisionResponse(PhysicalBody& body, const PhysicalBody& otherBody, const Vector2& normal, Timestep ts);
        bool checkBroadPhase(const PhysicalBody& body1, const PhysicalBody& body2);
        void checkNarrowPhase(const PhysicalBody& body1, const PhysicalBody& body2, CollisionInfo& info);

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
        }

        inline PhysicalBody* getPhysicalBody(BodyHandle identifier) { return &m_Bodies[identifier]; };
        inline const PhysicalBody& readPhysicalBody(BodyHandle identifier) const { return m_Bodies[identifier]; };
        void removePhysicalBody(BodyHandle body);

        void onUpdate(Timestep ts);
    };
}