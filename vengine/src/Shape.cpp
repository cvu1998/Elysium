#include "Shape.h"

std::array<Vertex, 4> Shape::createSquare(Vec2 position, float size, Vec4 color, float textureID)
{
    Vertex v0;
    v0.position = { position.x, position.y };
    v0.color = { color.x, color.y, color.z, color.a };
    v0.textureCoord = { 0.0f, 0.0f };
    v0.textureID = textureID;

    Vertex v1;
    v1.position = { position.x + size, position.y };
    v1.color = { color.x, color.y, color.z, color.a };
    v1.textureCoord = { 1.0f, 0.0f };
    v1.textureID = textureID;

    Vertex v2;
    v2.position = { position.x + size, position.y + size };
    v2.color = { color.x, color.y, color.z, color.a };
    v2.textureCoord = { 1.0f, 1.0f };
    v2.textureID = textureID;

    Vertex v3;
    v3.position = { position.x, position.y + size };
    v3.color = { color.x, color.y, color.z, color.a };
    v3.textureCoord = { 0.0f, 1.0f };
    v3.textureID = textureID;

    return { v0, v1, v2, v3 };
}