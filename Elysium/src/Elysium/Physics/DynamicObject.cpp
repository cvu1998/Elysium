#include "DynamicObject.h"

namespace Elysium
{
    DynamicObject::DynamicObject(const char* name, const glm::vec2& position, const glm::vec2& size, float mass) :
        PhysicalObject(name, position, size, ObjectType::DYNAMIC)
    {
        Mass = mass;
    }

    void DynamicObject::applyObjectLogic(Timestep ts)
    {
    }

    void DynamicObject::onCollision(const PhysicalObject* ObjectCollided, const ObjectCollisionInfo& info, const ObjectCollisionInfo& otherInfo, Timestep ts)
    {
    }

    void DynamicObject::onUpdate(Timestep ts)
    {
        Acceleration = Force / Mass;
        Velocity = Velocity + (Impulse / Mass) + (Acceleration * (float)ts);
        Position = Position + (Velocity * (float)ts);

        Impulse = { 0.0f, 0.0f };

        applyObjectLogic(ts);
    }
}