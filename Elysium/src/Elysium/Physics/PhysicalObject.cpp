#include "PhysicalObject.h"

namespace Elysium
{
    void PhysicalObject::onUpdate(Timestep ts, float time, float acceleration)
    {
        m_Position.x = InitialPosition.x + (InitialVelocity.x * time) + (0.5f * Acceleration.x * time * time);
        m_Position.y = InitialPosition.y + (InitialVelocity.y * time) + (0.5f * (Acceleration.y + acceleration) * time * time);

        Renderer2D::drawQuadWithRotation(m_Position, Size, glm::radians(Rotation * ts), TextureID, Color);
    }
}