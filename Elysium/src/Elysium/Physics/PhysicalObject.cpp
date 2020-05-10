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
        float distances[4];

        glm::vec2 MiddlePoint = {Position.x, Position.y - Size.y / 2};
        distances[0] = (MiddlePoint.x - object->getPosition().x) * (MiddlePoint.x - object->getPosition().x) +
            (MiddlePoint.y - object->getPosition().y) * (MiddlePoint.y - object->getPosition().y);

        MiddlePoint = { Position.x + Size.x / 2 , Position.y};
        distances[1] = (MiddlePoint.x - object->getPosition().x) * (MiddlePoint.x - object->getPosition().x) +
            (MiddlePoint.y - object->getPosition().y) * (MiddlePoint.y - object->getPosition().y);

        MiddlePoint = { Position.x, Position.y + Size.y / 2 };
        distances[2] = (MiddlePoint.x - object->getPosition().x) * (MiddlePoint.x - object->getPosition().x) +
            (MiddlePoint.y - object->getPosition().y) * (MiddlePoint.y - object->getPosition().y);

        MiddlePoint = { Position.x - Size.x / 2 , Position.y };
        distances[3] = (MiddlePoint.x - object->getPosition().x) * (MiddlePoint.x - object->getPosition().x) +
            (MiddlePoint.y - object->getPosition().y) * (MiddlePoint.y - object->getPosition().y);

        unsigned int index = 0;
        for (unsigned int i = 1; i < 4; i++)
        {
            if (distances[i] < distances[index])
                index = i;
        }

        if (index == 2)
            return CollisionOccurence::TOP;
        else if (index == 0)
            return CollisionOccurence::BOTTOM;
        else if (index == 3)
            return CollisionOccurence::LEFT;
        else
            return CollisionOccurence::RIGHT;
    }
}