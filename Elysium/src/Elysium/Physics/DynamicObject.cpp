#include "DynamicObject.h"

namespace Elysium
{
    DynamicObject::DynamicObject(const glm::vec2& position, const glm::vec2& size, float mass) :
        PhysicalObject(position, size, ObjectType::DYNAMIC)
    {
        Mass = mass;
    }

    glm::vec2 DynamicObject::getFuturePosition(const glm::vec2& position, Timestep ts) const
    {
        glm::vec2 futureAcceleration= Force / Mass;
        glm::vec2 futureVelocity = Velocity + (Impulse / Mass) + (futureAcceleration * (float)ts);
        return position + (futureVelocity * (float)ts);
    }

    void DynamicObject::onCollision()
    {
    }

    void DynamicObject::onUpdate(Timestep ts)
    {
        Acceleration = Force / Mass;
        Velocity = Velocity + (Impulse / Mass) + (Acceleration * (float)ts);
        Position = Position + (Velocity * (float)ts);

        VerticesPosition[0] = { Position.x - Size.x / 2, Position.y - Size.y / 2 };
        VerticesPosition[1] = { Position.x + Size.x / 2, Position.y - Size.y / 2 };
        VerticesPosition[2] = { Position.x + Size.x / 2, Position.y + Size.y / 2 };
        VerticesPosition[3] = { Position.x - Size.x / 2, Position.y + Size.y / 2 };

        Impulse = { 0.0f, 0.0f };
    }
}