#include "StaticObject.h"

namespace Elysium
{
    StaticObject::StaticObject(const char* name, const glm::vec2& position, const glm::vec2& size) : PhysicalObject(name, position, size, ObjectType::STATIC)
    {
    }

    void StaticObject::onCollision()
    {
    }

    void StaticObject::onUpdate(Timestep ts)
    {
    }
}
