#include "PhysicalBody.h"

namespace Elysium
{
    PhysicalBody::PhysicalBody(BodyType type,const char* name, float mass, const Vector2& initialPosition, const Vector2& size, 
        Collision_Callback callback) :
        Type(type),
        Name(name),
        Position(initialPosition),
        Size(size),
        BroadSize(std::max(size.x, size.y)),
        Callback(callback)
    {
        float halfLength = size.x / 2.0f;
        float halfWidth = size.y / 2.0f;
        
        m_ModelVertices.reserve(4);
        m_ModelVertices.emplace_back(-halfLength, -halfWidth);
        m_ModelVertices.emplace_back( halfLength, -halfWidth);
        m_ModelVertices.emplace_back( halfLength,  halfWidth);
        m_ModelVertices.emplace_back(-halfLength,  halfWidth);

        if (type == BodyType::DYNAMIC)
            Mass = mass;
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

        for (Vector2 vertex : m_ModelVertices)
            vertices.push_back(tranformVertex(vertex));

        return vertices;
    }

    std::vector<Vector2> PhysicalBody::getNormals() const
    {
        std::vector<Vector2> vertices = getVertices();
        std::vector<Vector2> normals;
        normals.emplace_back(-(vertices[0].y - vertices[1].y), vertices[0].x - vertices[1].x);
        normals.emplace_back(-(vertices[1].y - vertices[2].y), vertices[1].x - vertices[2].x);
        return normals;
    }
}