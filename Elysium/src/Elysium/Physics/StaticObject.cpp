#include "StaticObject.h"

namespace Elysium
{
    StaticObject::StaticObject(const char* name, const glm::vec2& position, const glm::vec2& size) : PhysicalObject(name, position, size, ObjectType::STATIC)
    {
    }

    void StaticObject::applyObjectLogic(Timestep ts)
    {
    }

    void StaticObject::onCollision(const PhysicalObject* ObjectCollided, const ObjectCollisionInfo& info, const ObjectCollisionInfo& otherInfo, Timestep ts)
    {
    }

    void StaticObject::onUpdate(Timestep ts)
    {
        Force = { 0.0f, 0.0f };
        Impulse = { 0.0f, 0.0f };

        Acceleration = { 0.0f, 0.0f };
        Velocity = { 0.0f, 0.0f };

        applyObjectLogic(ts);
    }
}
