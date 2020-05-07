#include "PhysicalObject.h"

namespace Elysium
{
    PhysicalObject::PhysicalObject(glm::vec2& initialPosition, ObjectType type) : 
        Position(initialPosition),
        m_Type(type)
    {
    }

    bool PhysicalObject::isBoxColliding(const PhysicalObject* object) const
    {
        float XObject = object->getPosition().x;
        float XObjectHalfSize = object->Size.x / 2;

        bool CollisionX =
            (Position.x + Size.x >=
                XObject - XObjectHalfSize &&
                Position.x - Size.x <=
                XObject + XObjectHalfSize);

        float YObject = object->getPosition().y;
        float YObjectHalfSize = object->Size.y / 2;

        bool CollisionY =
            (Position.y + Size.y >=
                YObject - YObjectHalfSize &&
                Position.y - Size.y <=
                YObject + YObjectHalfSize);

        return CollisionX && CollisionY;
    }
}