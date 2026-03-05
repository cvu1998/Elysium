#pragma once

#include <atomic>
#include <deque>
#include <mutex>
#include <thread>
#include <unordered_set>

#include "Elysium/Math.h"
#include "Elysium/Physics/PhysicalBody2D.h"
#include "Elysium/Utility.h"

namespace Elysium
{
    using BodyHandle = unsigned short;

    const float EPSILON = 1e-6f;

    class PhysicsSystem2D final
    {
    private:
        std::deque<PhysicalBody2D> m_Bodies;
        std::vector<BodyHandle> m_InactiveBodies;

        std::thread m_UpdateThread;
        std::mutex m_UpdateMutex;
        unsigned long long m_LastFrameID = 0;
        std::atomic<bool> m_RunUpdateThread;
        std::atomic<bool> m_Shutdown = false;

    public:
        bool Gravity = true;
        float GravitationalAccel = -9.80f;

    private:
        PhysicsSystem2D(bool useThread);
        ~PhysicsSystem2D();

        PhysicsSystem2D(const PhysicsSystem2D&) = delete;
        PhysicsSystem2D(PhysicsSystem2D&&) = delete;
        PhysicsSystem2D& operator=(const PhysicsSystem2D&) = delete;
        PhysicsSystem2D& operator=(PhysicsSystem2D&&) = delete;

        bool checkBroadPhase(const PhysicalBody2D& body1, const PhysicalBody2D& body2);
        void checkNarrowPhase(const PhysicalBody2D& body1, const PhysicalBody2D& body2, CollisionInfo& info);

        Vector2 computeContactPoint(
            const PhysicalBody2D& bodyI,
            const PhysicalBody2D& bodyJ,
            const Vector2& iNormal
        );

        void resolveCollision(
            PhysicalBody2D& bodyI,
            PhysicalBody2D& bodyJ,
            const Vector2& iNormal,
            float overlap,
            Timestep ts
        );

        void printInfo(BodyHandle i);

        void Update();

    public:
        template<UpdateDevice T>
        static void Init();

        template<>
        static void Init<UpdateDevice::CPU>();

        template<>
        static void Init<UpdateDevice::CPU_THREAD>();

        static void Shutdown();

        static PhysicsSystem2D& Get();

        PhysicalBody2D* createPhysicalBody(
            BodyType type,
            Collider collider,
            const char* name,
            float mass,
            const Vector2& initialPosition,
            const Vector2& size,
            PhysicalBody2D::Collision_Callback&& callback = nullptr
        );

        void createPhysicalBody(
            BodyHandle* handle,
            BodyType type,
            Collider collider,
            const char* name,
            float mass,
            const Vector2& initialPosition,
            const Vector2& size,
            PhysicalBody2D::Collision_Callback&& callback = nullptr
        );

        void removePhysicalBody(BodyHandle body);

        PhysicalBody2D* getPhysicalBody(BodyHandle identifier) { return &m_Bodies[identifier]; };
        const PhysicalBody2D& readPhysicalBody(BodyHandle identifier) const { return m_Bodies[identifier]; };

        std::deque<PhysicalBody2D>& getBodies() { return m_Bodies; }
        void clear()
        {
            std::scoped_lock lock(m_UpdateMutex);
            m_Bodies.clear();
            m_InactiveBodies.clear();
        }

        std::mutex& getUpdateMutex() { return m_UpdateMutex; }

        void runThread(bool runThread) { m_RunUpdateThread.store(runThread, std::memory_order_release); }

        void logInfo(const char* tag);

        void onUpdate(Timestep ts);
    };
}