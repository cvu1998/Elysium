#pragma once

#include <fstream>
#include <unordered_map>

#include <glm/glm.hpp>

#include "Elysium/Physics/PhysicalObject.h"

namespace Elysium
{
    class PhysicsSystem
    {
    private:
        float m_Time = 0.0f;
        float m_GravitationalAccel;

        Timestep m_CurrentTimestep;

        struct Hash_ObjectPair
        {
            size_t operator()(const std::pair<PhysicalObject*, PhysicalObject*>& pair) const
            {
                auto hash1 = std::hash<PhysicalObject*>{}(pair.first);
                auto hash2 = std::hash<PhysicalObject*>{}(pair.second);
                return hash1 ^ hash2;
            }
        };

        unsigned int m_ObjectInsertIndex = 0;
        std::vector<PhysicalObject*> m_Objects;
        std::unordered_map<std::pair<PhysicalObject*, PhysicalObject*>, bool, Hash_ObjectPair> m_CollisionMap;

        OrthographicCamera* m_Camera;

        #ifdef _DEBUG
        std::ofstream Logfile;
        #endif

    private:
        bool checkFutureBoxCollision(const PhysicalObject* object1, const PhysicalObject* object2);

    public:
        PhysicsSystem(float acceleration, OrthographicCamera& camera);
        ~PhysicsSystem();

        inline float getGravitaionnalAccel() const { return m_GravitationalAccel; }
        inline Timestep getTimeStep() const { return m_CurrentTimestep; }

        inline void setGravitaionnalAccel(float acceleration) { m_GravitationalAccel = acceleration; }

        void addPhysicalObject(PhysicalObject* object);
        void removePhysicalObject(PhysicalObject* object);

        bool areColliding(PhysicalObject* object1, PhysicalObject* object2);

        void onUpdate(Timestep ts);
    };
}