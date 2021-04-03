#include "PhysicalBody2D.h"

#include <unordered_set>

namespace Elysium
{
    PhysicalBody2D::PhysicalBody2D(BodyType type, Collider collider, const char* tag, float mass,
        const Vector2& initialPosition, const Vector2& size,
        Collision_Callback callback) :
        Type(type),
        Model(collider),
        Tag(tag),
        Position(initialPosition),
        Size(size),
        Callback(callback)
    {
        float halfLength = size.x * 0.5f;
        float halfWidth = size.y * 0.5f;

        switch (collider)
        {
        case Collider::QUAD:
            m_ModelVertices.reserve(4);
            m_ModelVertices.emplace_back(-halfLength, -halfWidth);
            m_ModelVertices.emplace_back( halfLength, -halfWidth);
            m_ModelVertices.emplace_back( halfLength,  halfWidth);
            m_ModelVertices.emplace_back(-halfLength,  halfWidth);

            m_Normals.reserve(4);
            m_Normals.emplace_back( 0.0f, -1.0f);
            m_Normals.emplace_back( 1.0f,  0.0f);
            m_Normals.emplace_back( 0.0f,  1.0f);
            m_Normals.emplace_back(-1.0f,  0.0f);

            Normals = m_Normals;
            break;
        case Collider::CIRCLE:
            break;
        }

        if (type == BodyType::DYNAMIC || type == BodyType::KINEMATIC)
        {
            Mass = fabs(mass);
            Inertia = (size.x * size.y * size.y * size.y) / 12.0f;
        }
    }

    void PhysicalBody2D::setElasticityCoefficient(float coefficient)
    {
        if (coefficient >= 0.0f && coefficient <= 1.0f)
            ElasticityCoefficient = 1.0f + coefficient;
    }

    void PhysicalBody2D::setFrictionCoefficient(float coefficient)
    {
        if (coefficient >= 0.0f && coefficient <= 1.0f)
            FrictionCoefficient = coefficient;
    }

    Vector2 PhysicalBody2D::tranformVertex(const Vector2& vertex) const
    {
        Complex transform(glm::cos(Rotation), glm::sin(Rotation));
        Complex translation(Position.x, Position.y);

        return (Vector2)(Complex(vertex.x, vertex.y) * transform + translation);
    }

    void PhysicalBody2D::getVertices(std::vector<Vector2>& vertices) const
    {
        for (const Vector2& vertex : m_ModelVertices)
            vertices.push_back(tranformVertex(vertex));
    }

    Vector2 PhysicalBody2D::getMaxVertex() const
    {
        Vector2 halfSize = Size * 0.5f;
        Vector2 array[4] = 
        {
            tranformVertex(+halfSize),
            tranformVertex(-halfSize),
            tranformVertex({ +halfSize.x, -halfSize.y}),
            tranformVertex({ -halfSize.x, +halfSize.y })
        };

        for (size_t i = 1; i < 4; i++)
        {
            if (array[i].x > array[0].x)
                array[0].x = array[i].x;
            if (array[i].y > array[0].y)
                array[0].y = array[i].y;
        }
        return array[0];
    }

    Vector2 PhysicalBody2D::getMinVertex() const
    {
        Vector2 halfSize = Size * 0.5f;
        Vector2 array[4] =
        {
            tranformVertex(+halfSize),
            tranformVertex(-halfSize),
            tranformVertex({ +halfSize.x, -halfSize.y}),
            tranformVertex({ -halfSize.x, +halfSize.y })
        };

        for (size_t i = 1; i < 4; i++)
        {
            if (array[i].x < array[0].x)
                array[0].x = array[i].x;
            if (array[i].y < array[0].y)
                array[0].y = array[i].y;
        }
        return array[0];
    }

    void PhysicalBody2D::updateNormals()
    {
        Complex transform(glm::cos(Rotation), glm::sin(Rotation));
        for (size_t i = 0; i < Normals.size(); ++i)
            Normals[i] = glm::normalize((Vector2)(Complex(m_Normals[i].x, m_Normals[i].y) * transform));
    }

    void PhysicalBody2D::resetValues()
    {
        Velocity = { 0.0f, 0.0f };
        Acceleration = { 0.0f, 0.0f };
        Rotation = 0.0f;
    }
}