#include "PhysicalBody.h"

#include <set>

namespace Elysium
{
    PhysicalBody::PhysicalBody(BodyType type,const char* name, float mass, const Vector2& initialPosition, const Vector2& size, 
        Collision_Callback callback) :
        Type(type),
        Name(name),
        Position(initialPosition),
        Size(size),
        Callback(callback)
    {
        float halfLength = size.x * 0.5f;
        float halfWidth = size.y * 0.5f;
        
        m_ModelVertices.reserve(4);
        m_ModelVertices.emplace_back(-halfLength, -halfWidth);
        m_ModelVertices.emplace_back( halfLength, -halfWidth);
        m_ModelVertices.emplace_back( halfLength,  halfWidth);
        m_ModelVertices.emplace_back(-halfLength,  halfWidth);

        if (type == BodyType::DYNAMIC)
            Mass = fabs(mass);
    }

    PhysicalBody PhysicalBody::createPhysicalBody(BodyType type, const char* name, float mass, const Vector2& initialPosition, const Vector2& size, Collision_Callback callback)
    {
        return PhysicalBody(type, name, mass, initialPosition, size, callback);
    }

    void PhysicalBody::setRadius(float radius)
    {
        if (radius > 0.0f)
        {
            Radius = radius;
            Size = { 2.0f * radius, 2.0f * radius };
        }

    }

    void PhysicalBody::setElasticityCoefficient(float coefficient)
    {
        if (coefficient >= 0.0f && coefficient <= 1.0f)
            ElasticityCoefficient = 1.0f + coefficient;
    }

    void PhysicalBody::setFrictionCoefficient(float coefficient)
    {
        if (coefficient >= 0.0f && coefficient <= 1.0f)
            FrictionCoefficient = coefficient;
    }

    Vector2 PhysicalBody::tranformVertex(const Vector2& vertex) const
    {
        float radiansRotation = radians(Rotation);
        Complex transform(cos(radiansRotation), sin(radiansRotation));
        Complex translation(Position.x, Position.y);

        return (Vector2)(Complex(vertex.x, vertex.y) * transform + translation);
    }

    std::vector<Vector2> PhysicalBody::getVertices() const
    {
        std::vector<Vector2> vertices;

        for (const Vector2& vertex : m_ModelVertices)
            vertices.push_back(vertex + Position);
            //vertices.push_back(tranformVertex(vertex));

        return vertices;
    }

    Vector2 PhysicalBody::getMinVertex() const
    {
        Vector2 array[4];
        array[0] = tranformVertex(+Size * 0.5f);
        array[1] = tranformVertex(-Size * 0.5f);
        array[2] = tranformVertex({ +Size.x * 0.5f, -Size.y * 0.5f });
        array[3] = tranformVertex({ -Size.x * 0.5f, +Size.y * 0.5f });

        for (size_t i = 1; i < 4; i++)
        {
            if (array[i].x < array[0].x)
                array[0].x = array[i].x;
            if (array[i].y < array[0].y)
                array[0].y = array[i].y;
        }
        return array[0];
    }

    Vector2 PhysicalBody::getMaxVertex() const
    {
        Vector2 array[4];
        array[0] = tranformVertex(+Size * 0.5f);
        array[1] = tranformVertex(-Size * 0.5f);
        array[2] = tranformVertex({ +Size.x * 0.5f, -Size.y * 0.5f });
        array[3] = tranformVertex({ -Size.x * 0.5f, +Size.y * 0.5f });

        for (size_t i = 1; i < 4; i++)
        {
            if (array[i].x > array[0].x)
                array[0].x = array[i].x;
            if (array[i].y > array[0].y)
                array[0].y = array[i].y;
        }
        return array[0];
    }

    std::vector<Vector2> PhysicalBody::getNormals() const
    {
        std::vector<Vector2> vertices = getVertices();
        std::vector<Vector2> normals;

        std::set<float> polarAngles;
        std::pair<std::set<float>::iterator, bool> valid;
        if (vertices.size() > 0)
        {
            for (size_t i = 0; i < vertices.size() - 1; i++)
            {
                Vector2 normal = { -(vertices[i].y - vertices[i + 1].y), vertices[i].x - vertices[i + 1].x };
                valid = polarAngles.insert(fabs(atan2(normal.y, normal.x)));
                if (valid.second)
                    normals.push_back(normal);
            }
        }
        return normals;
    }
}