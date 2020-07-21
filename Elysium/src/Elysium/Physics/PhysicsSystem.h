#pragma once

#include <fstream>
#include <limits>
#include <unordered_map>

#include "Elysium/Math.h"
#include "Elysium/Physics/PhysicalBody.h"

#define LOG

namespace Elysium
{
    typedef std::uint32_t PhysicsBody;

    class PhysicsSystem final
    {
    private:
        float m_Time = 0.0f;
        float m_GravitationalAccel;

        std::vector<PhysicalBody> m_Bodies;
        std::vector<PhysicsBody> m_InactiveBodies;

#ifdef LOG
        std::ofstream m_LogFile;
#endif

    public:
        bool Gravity = true;

    private:
        bool checkBroadPhase(const PhysicalBody& body1, const PhysicalBody& body2);
        void checkNarrowPhase(const PhysicalBody& body1, const PhysicalBody& body2, CollisionInfo& info);

    public:
        PhysicsSystem(float acceleration);
        ~PhysicsSystem();

        PhysicsBody createPhysicalBody(BodyType type, const char* name, float mass, const Vector2& initialPosition, const Vector2& size,
            PhysicalBody::Collision_Callback callback = nullptr);

        inline void setGravitaionnalAccel(float acceleration) { m_GravitationalAccel = acceleration; }
        inline float getGravitaionnalAccel() const { return m_GravitationalAccel; }

        inline std::vector<PhysicalBody>& getBodies() { return m_Bodies; }

        inline PhysicalBody& getPhysicalBody(PhysicsBody identifier) { return m_Bodies[identifier]; };
        void removePhysicalBody(PhysicsBody body);

        void onUpdate(Timestep ts);
    };
}