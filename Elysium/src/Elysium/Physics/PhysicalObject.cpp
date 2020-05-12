#include "PhysicalObject.h"

namespace Elysium
{
    PhysicalObject::PhysicalObject(const glm::vec2& initialPosition, const glm::vec2& size, ObjectType type) :
        Position(initialPosition),
        Size(size),
        m_Type(type)
    {
        VerticesPosition[0] = { initialPosition.x - size.x / 2, initialPosition.y - size.y / 2 };
        VerticesPosition[1] = { initialPosition.x + size.x / 2, initialPosition.y - size.y / 2 };
        VerticesPosition[2] = { initialPosition.x + size.x / 2, initialPosition.y + size.y / 2 };
        VerticesPosition[3] = { initialPosition.x - size.x / 2, initialPosition.y + size.y / 2 };
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

    void PhysicalObject::Draw()
    {
        Renderer2D::drawQuad(VerticesPosition, Size, TextureID, Color);
    }

    CollisionOccurence PhysicalObject::getCollisionOccurence(const PhysicalObject* object) const
    {
        if ((object->getPosition().y - (object->Size.y / 2) <= Position.y + Size.y) && (
            (VerticesPosition[3].x <= object->getVerticesPosition()[1].x && VerticesPosition[2].x >= object->getVerticesPosition()[1].x) ||
            (VerticesPosition[3].x <= object->getVerticesPosition()[0].x && VerticesPosition[2].x >= object->getVerticesPosition()[0].x)))
            return CollisionOccurence::TOP;

        if ((object->getPosition().y + (object->Size.y / 2) >= Position.y - Size.y) && (
            (VerticesPosition[0].x <= object->getVerticesPosition()[3].x && VerticesPosition[1].x >= object->getVerticesPosition()[3].x) ||
            (VerticesPosition[0].x <= object->getVerticesPosition()[2].x && VerticesPosition[1].x >= object->getVerticesPosition()[2].x)))
            return CollisionOccurence::BOTTOM;

        if ((object->getPosition().x + (object->Size.x / 2) > Position.x - Size.x) && (
            (VerticesPosition[1].y <= object->getVerticesPosition()[3].y && VerticesPosition[2].y >= object->getVerticesPosition()[3].y) ||
            (VerticesPosition[1].y <= object->getVerticesPosition()[0].y && VerticesPosition[2].y >= object->getVerticesPosition()[0].y)))
            return CollisionOccurence::RIGHT;

        if ((object->getPosition().x - (object->Size.x / 2) < Position.x + Size.x) && (
            (VerticesPosition[0].y <= object->getVerticesPosition()[2].y && VerticesPosition[3].y >= object->getVerticesPosition()[2].y) ||
            (VerticesPosition[0].y <= object->getVerticesPosition()[1].y && VerticesPosition[3].y >= object->getVerticesPosition()[1].y)))
            return CollisionOccurence::LEFT;

        return CollisionOccurence::NONE;
    }
}