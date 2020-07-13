#include "PhysicalObject.h"

namespace Elysium
{
    PhysicalObject::PhysicalObject(const char* name, const Vector2& initialPosition, const Vector2& size, ObjectType type) :
        Name(name),
        Position(initialPosition),
        Size(size),
        BroadSize(std::max(size.x, size.y)),
        m_Type(type)
    {
        float halfLength = size.x / 2.0f;
        float halfWidth = size.y / 2.0f;
        
        m_ModelVertices.reserve(4);
        m_ModelVertices.emplace_back(-halfLength, -halfWidth);
        m_ModelVertices.emplace_back( halfLength, -halfWidth);
        m_ModelVertices.emplace_back( halfLength,  halfWidth);
        m_ModelVertices.emplace_back(-halfLength,  halfWidth);
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

    Vector2 PhysicalObject::tranformVertex(const Vector2& vertex) const
    {
        Complex transform(cos(Rotation), sin(Rotation));
        Complex translation(Position.x, Position.y);

        return (Vector2) (Complex(vertex.x, vertex.y) * transform + translation);
    }

    std::vector<Vector2> PhysicalObject::getModelVertices() const
    {
        std::vector<Vector2> vertices;

        for (Vector2 vertex : m_ModelVertices)
            vertices.push_back(tranformVertex(vertex));

        return vertices;
    }

    std::vector<Vector2> PhysicalObject::getNormals() const
    {
        std::vector<Vector2> vertices = getModelVertices();
        std::vector<Vector2> normals;
        normals.emplace_back(-(vertices[0].y - vertices[1].y), vertices[0].x - vertices[1].x);
        normals.emplace_back(-(vertices[1].y - vertices[2].y), vertices[1].x - vertices[2].x);
        return normals;
    }

    void PhysicalObject::Draw()
    {
       if (!TextureData.isDefault())
           Renderer2D::drawQuad(Position, Size, TextureData, Color);
       else
           Renderer2D::drawQuad(Position, Size, Color);
    }
}