#pragma once

#include <deque>
#include <memory>
#include <mutex>
#include <thread>
#include <unordered_set>

#include "Elysium/Math.h"
#include "Elysium/Physics/PhysicalBody2D.h"
#include "Elysium/Utility.h"

namespace Elysium
{
    class PhysicsSystem2D final
    {
    private:
        std::vector<PhysicalBody2D> m_Bodies;
        std::vector<BodyID> m_IdPool;
        std::vector<BodyID> m_BodyIndices;

        std::thread m_UpdateThread;
        uint64_t m_LastFrameID = 0;

        std::mutex m_UpdateMutex;
        bool m_RunUpdateThread = false;
        alignas(std::hardware_destructive_interference_size * 2) std::atomic<bool> m_Shutdown = false;

    public:
        inline static constexpr float EPSILON = 1e-6f;

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

        void printBodyInfo(size_t index);

        void Update();

    public:
        template<UpdateDevice D>
        static void Init();

        template<>
        static void Init<UpdateDevice::CPU>();

        template<>
        static void Init<UpdateDevice::CPU_THREAD>();

        static void Shutdown();

        static PhysicsSystem2D& Get();

        BodyID createPhysicalBody(
            BodyType type,
            Collider collider,
            const char* name,
            float mass,
            const Vector2& initialPosition,
            const Vector2& size,
            PhysicalBody2D::Collision_Callback&& callback = nullptr
        );

        void removePhysicalBody(BodyID id);

        PhysicalBody2D& getBody(BodyID id) { return m_Bodies[m_BodyIndices[id]]; }

        void clear()
        {
            m_Bodies.clear();
            m_IdPool.clear();
            m_BodyIndices.clear();
        }

        std::mutex& getUpdateMutex() { return m_UpdateMutex; }

        bool isRunningThread() { return m_RunUpdateThread; }

        void onUpdate(Timestep ts);
    };
}