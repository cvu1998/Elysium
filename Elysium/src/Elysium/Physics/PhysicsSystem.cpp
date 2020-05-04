#include "Elysium/Physics/PhysicsSystem.h"

namespace Elysium
{
    PhysicsSystem::PhysicsSystem(float acceleration, OrthographicCamera& camera) : m_GravitationnalAccel(-1.0f * acceleration),
        m_Camera(&camera)
    {
    }

    void PhysicsSystem::removePhysicalObject(PhysicalObject* object)
    {
        auto it = std::find(m_Objects.begin(), m_Objects.end(), object);
        if (it != m_Objects.end())
            m_Objects.erase(it);
    }

    void PhysicsSystem::onUpdate(Timestep ts)
    {
        m_Time += ts;

        Renderer2D::beginScene(*m_Camera);
        for (auto object : m_Objects)
        {
            object->onUpdate(ts, m_Time, m_GravitationnalAccel);
        }
        Renderer2D::endScene();
    }
}