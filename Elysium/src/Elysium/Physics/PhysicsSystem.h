#pragma once

#include <fstream>

#include <glm/glm.hpp>

#include "Elysium/Physics/PhysicalObject.h"

namespace Elysium
{
    class PhysicsSystem
    {
    private:
        float m_Time = 0.0f;
        float m_GravitationnalAccel;

        Timestep m_CurrentTimestep;

        std::vector<PhysicalObject*> m_Objects;
        unsigned int m_ObjectInsertIndex = 0;;

        OrthographicCamera* m_Camera;

        std::ofstream m_Logfile;

    public:
        PhysicsSystem(float acceleration, OrthographicCamera& camera);
        ~PhysicsSystem();

        inline float getGravitaionnalAccel() const { return m_GravitationnalAccel; }
        inline Timestep getTimeStep() const { return m_CurrentTimestep; }

        inline void setGravitaionnalAccel(float acceleration) { m_GravitationnalAccel = acceleration; }

        void addPhysicalObject(PhysicalObject* object);
        void removePhysicalObject(PhysicalObject* object);

        bool checkFutureBoxCollision(const PhysicalObject* object1, const PhysicalObject* object2);

        void onUpdate(Timestep ts);
    };
}