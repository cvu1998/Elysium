#pragma once

#include <glm/glm.hpp>

#include "Elysium/Physics/PhysicalObject.h"

namespace Elysium
{
    class PhysicsSystem
    {
    private:
        float m_Time = 0.0f;
        float m_GravitationnalAccel;
        std::vector<PhysicalObject*> m_Objects;

        OrthographicCamera* m_Camera;

    public:
        PhysicsSystem(float acceleration, OrthographicCamera& camera);

        inline float getGravitaionnalAccel() const { return m_GravitationnalAccel; }
        inline void setGravitaionnalAccel(float acceleration) { m_GravitationnalAccel = -1.0f * acceleration; }
        inline void addPhysicalObject(PhysicalObject* object) { m_Objects.push_back(object); }

        void removePhysicalObject(PhysicalObject* object);

        void onUpdate(Timestep ts);
    };
}