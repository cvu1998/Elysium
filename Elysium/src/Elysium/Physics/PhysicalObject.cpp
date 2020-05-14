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

    const glm::vec2& PhysicalObject::getMinVertex() const
    {
        unsigned int index = 0;
        for (unsigned int i = 1; i < 4; i++)
        {
            if (VerticesPosition[i].x < VerticesPosition[index].x || VerticesPosition[i].y < VerticesPosition[index].y)
                index = i;
        }
        return VerticesPosition[index];
    }

    const glm::vec2& PhysicalObject::getMaxVertex() const
    {
        unsigned int index = 0;
        for (unsigned int i = 1; i < 4; i++)
        {
            if (VerticesPosition[i].x > VerticesPosition[index].x || VerticesPosition[i].y > VerticesPosition[index].y)
                index = i;
        }
        return VerticesPosition[index];
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
       if (texture)
           Renderer2D::drawQuad(Position, Size, *texture, Color);
       else
           Renderer2D::drawQuad(Position, Size, Color);
    }

    CollisionOccurence PhysicalObject::getCollisionOccurence(const PhysicalObject* object) const
    {
        glm::vec2 ObjectMax = object->getMaxVertex();
        glm::vec2 ObjectMin = object->getMinVertex();

        glm::vec2 max = getMaxVertex();
        glm::vec2 min = getMinVertex();

        if (max.x >= object->getPosition().x && min.x <= object->getPosition().x && (max.y + Size.y) >= object->getPosition().y && (min.y + Size.y) <= object->getPosition().y)
            return CollisionOccurence::TOP;
        else if ((max.x - Size.x) >= object->getPosition().x && (min.x - Size.x) <= object->getPosition().x && max.y >= object->getPosition().y && min.y <= object->getPosition().y)
            return CollisionOccurence::LEFT;
        else if (max.x >= object->getPosition().x && min.x <= object->getPosition().x && (max.y - Size.y) >= object->getPosition().y && (min.y - Size.y) <= object->getPosition().y)
            return CollisionOccurence::BOTTOM;
        else if ((max.x + Size.x) >= object->getPosition().x && (min.x + Size.x) <= object->getPosition().x && max.y >= object->getPosition().y && min.y <= object->getPosition().y)
            return CollisionOccurence::RIGHT;
        return CollisionOccurence::NONE;
    }
}