#include "StaticObject.h"

namespace Elysium
{
    StaticObject::StaticObject(glm::vec2&& position) : PhysicalObject(position, ObjectType::STATIC)
    {
    }

    glm::vec2 StaticObject::getFuturePosition(Timestep ts) const
    {
        return Position;
    }

    void StaticObject::onCollision()
    {
    }

    void StaticObject::onUpdate(Timestep ts)
    {
        Renderer2D::drawQuadWithRotation(Position, Size, glm::radians(Rotation), TextureID, Color);
    }
}
