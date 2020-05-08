#include "PhysicalObject.h"

namespace Elysium
{
    PhysicalObject::PhysicalObject(glm::vec2& initialPosition, ObjectType type) :
        Position(initialPosition),
        m_Type(type)
    {
    }

    void PhysicalObject::setElasticityCoefficient(float coefficient)
    {
        if (coefficient >= 0.0f && coefficient <= 1.0f)
            ElasticityCoefficient = 1.0f + coefficient;
    }

    void PhysicalObject::setFrictionCoefficient(float coefficient)
    {
        if (coefficient >= 0.0f && coefficient <= 1.0f)
            FrictionCoefficient = coefficient;
    }

    CollisionOccurence PhysicalObject::getCollisionOccurence(const PhysicalObject* object) const
    {
        if (object->getPosition().y - (object->Size.y / 2) * cos(glm::radians(Rotation)) >= Position.y + (Size.y / 2) * cos(glm::radians(Rotation)))
            return CollisionOccurence::TOP;

        if (object->getPosition().y + (object->Size.y / 2) * cos(glm::radians(Rotation)) <= Position.y - (Size.y / 2) * cos(glm::radians(Rotation)))
            return CollisionOccurence::BOTTOM;

        if (object->getPosition().x > Position.x)
            return CollisionOccurence::RIGHT;

        if (object->getPosition().x < Position.x)
            return CollisionOccurence::LEFT;
    }
}