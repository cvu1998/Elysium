#include "StaticObject.h"

namespace Elysium
{
    StaticObject::StaticObject(const glm::vec2& position, const glm::vec2& size) : PhysicalObject(position, size, ObjectType::STATIC)
    {
    }

    glm::vec2 StaticObject::getFuturePosition(const glm::vec2& position, Timestep ts) const
    {
        return position;
    }

    void StaticObject::onCollision()
    {
    }

    void StaticObject::onUpdate(Timestep ts)
    {
    }
}
