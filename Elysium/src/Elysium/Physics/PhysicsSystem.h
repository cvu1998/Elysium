#pragma once

#include <fstream>
#include <map>
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

        unsigned int m_ObjectInsertIndex = 0;
        std::vector<PhysicalObject*> m_Objects;
        std::map<std::pair<unsigned int, unsigned int>, bool> m_CollisionMap;

        OrthographicCamera* m_Camera;

        std::ofstream Logfile;
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