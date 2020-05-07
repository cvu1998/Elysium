#include "DynamicObject.h"

namespace Elysium
{
    DynamicObject::DynamicObject(glm::vec2&& position) : PhysicalObject(position, ObjectType::DYNAMIC)
    {
    }

    glm::vec2 DynamicObject::getFuturePosition(Timestep ts) const
    {
        glm::vec2 futureVelocity;
        futureVelocity.x = Velocity.x + (LastAcceleration.x * ts);
        futureVelocity.y = Velocity.y + (LastAcceleration.y * ts);

        return { Position.x + (futureVelocity.x * ts) + (0.5f * LastAcceleration.x * ts * ts),
            Position.y + (futureVelocity.y * ts) + (0.5f * LastAcceleration.y * ts * ts) };
    }

    void DynamicObject::onCollision()
    {
    }

    void DynamicObject::onUpdate(Timestep ts)
    {
        Velocity.x = Velocity.x + (Acceleration.x * ts);
        Velocity.y = Velocity.y + (Acceleration.y * ts);

        Position.x = Position.x + (Velocity.x * ts) + (0.5f * Acceleration.x * ts * ts);
        Position.y = Position.y + (Velocity.y * ts) + (0.5f * Acceleration.y * ts * ts);

        LastAcceleration.x = Acceleration.x;    // Force Applied
        LastAcceleration.y = Acceleration.y;    // Gravity + Force Applied
        Acceleration = { 0.0f, 0.0f };

        Renderer2D::drawQuadWithRotation(Position, Size, glm::radians(Rotation * ts), TextureID, Color);
    }
}