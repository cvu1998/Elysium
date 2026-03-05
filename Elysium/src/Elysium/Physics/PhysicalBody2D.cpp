#include "PhysicalBody2D.h"

#include <unordered_set>

#include "Elysium/MathUtility.h"

namespace Elysium
{
    PhysicalBody2D::PhysicalBody2D(
        BodyType type,
        Collider collider,
        const char* tag,
        float mass,
        const Vector2& initialPosition,
        const Vector2& size,
        Collision_Callback&& callback
    ) :
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

        if (type == BodyType::DYNAMIC)
        {
            Mass = fabs(mass);
            InverseMass = 1.0f / Mass;
            Inertia = Mass * (size.x * size.x + size.y * size.y) / 12.0f;
        }
    }

    void PhysicalBody2D::setElasticityCoefficient(float coefficient)
    {
        if (coefficient >= 0.0f && coefficient <= 1.0f) ElasticityCoefficient = 1.0f + coefficient;
    }

    void PhysicalBody2D::setFrictionCoefficient(float coefficient)
    {
        if (coefficient >= 0.0f && coefficient <= 1.0f) FrictionCoefficient = coefficient;
    }

    Vector2 PhysicalBody2D::tranformVertex(const Vector2& vertex) const
    {
        Complex transform(glm::cos(Rotation), glm::sin(Rotation));
        Complex translation(Position.x, Position.y);

        return (Vector2)(Complex(vertex.x, vertex.y) * transform + translation);
    }

    void PhysicalBody2D::getVertices(std::array<Vector2, 4>& vertices) const
    {
        Complex transform(glm::cos(Rotation), glm::sin(Rotation));
        Complex translation(Position.x, Position.y);
        for (size_t i = 0; i < m_ModelVertices.size(); ++i)
        {
            vertices[i] = static_cast<Vector2>(
                Complex(m_ModelVertices[i].x, m_ModelVertices[i].y) * transform + translation
            );
        }
    }

    void PhysicalBody2D::updateBoundsAndNormals()
    {
        Complex transform(glm::cos(Rotation), glm::sin(Rotation));
        Complex translation(Position.x, Position.y);

        for (size_t i = 0; i < Normals.size(); ++i)
        {
            Normals[i] = glm::normalize(
                static_cast<Vector2>(Complex(m_Normals[i].x, m_Normals[i].y) * transform)
            );
        }

        if (!m_ModelVertices.empty())
        {
            Vector2 v0 = static_cast<Vector2>(
                Complex(m_ModelVertices[0].x, m_ModelVertices[0].y) * transform + translation
            );
            MaximumVertex = v0;
            MinimumVertex = v0;
            for (size_t i = 1; i < m_ModelVertices.size(); ++i)
            {
                Vector2 v = static_cast<Vector2>(
                    Complex(m_ModelVertices[i].x, m_ModelVertices[i].y) * transform + translation
                );
                if (v.x > MaximumVertex.x) MaximumVertex.x = v.x;
                if (v.y > MaximumVertex.y) MaximumVertex.y = v.y;
                if (v.x < MinimumVertex.x) MinimumVertex.x = v.x;
                if (v.y < MinimumVertex.y) MinimumVertex.y = v.y;
            }
        }
        else
        {
            // Circle: compute AABB from the 4 axis-extreme points
            Vector2 halfSize = Size * 0.5f;
            Vector2 corners[4] = {
                static_cast<Vector2>(Complex(+halfSize.x, +halfSize.y) * transform + translation),
                static_cast<Vector2>(Complex(-halfSize.x, -halfSize.y) * transform + translation),
                static_cast<Vector2>(Complex(+halfSize.x, -halfSize.y) * transform + translation),
                static_cast<Vector2>(Complex(-halfSize.x, +halfSize.y) * transform + translation)
            };
            MaximumVertex = corners[0];
            MinimumVertex = corners[0];
            for (int i = 1; i < 4; i++)
            {
                if (corners[i].x > MaximumVertex.x) MaximumVertex.x = corners[i].x;
                if (corners[i].y > MaximumVertex.y) MaximumVertex.y = corners[i].y;
                if (corners[i].x < MinimumVertex.x) MinimumVertex.x = corners[i].x;
                if (corners[i].y < MinimumVertex.y) MinimumVertex.y = corners[i].y;
            }
        }
    }

    void PhysicalBody2D::onUpdate(Timestep ts)
    {
        ContactNormal = Math::normalize(ContactNormal);
        Impulse += ContactImpulse * ContactNormal;

        Normal += ContactNormal;
        Normal = Math::normalize(Normal);

        Acceleration = Force * InverseMass;
        Velocity = Velocity + (Impulse * InverseMass) + (Acceleration * static_cast<float>(ts));
        Position = Position + (Velocity * static_cast<float>(ts));

        Force = { 0.0f, 0.0f };
    }

    void PhysicalBody2D::resetValues()
    {
        ContactImpulse = { 0.0f, 0.0f };
        ContactNormal = { 0.0f, 0.0f };

        Force = { 0.0f, 0.0f };
        Impulse = { 0.0f, 0.0f };

        Velocity = { 0.0f, 0.0f };
        Acceleration = { 0.0f, 0.0f };
        Rotation = 0.0f;
    }
}