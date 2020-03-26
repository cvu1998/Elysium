#include "Shape.h"

Vertex* Shape::createSquare(Vertex* target, glm::vec2 position, float size, glm::vec4 color, float textureID)
{
    target->position = { position.x, position.y };
    target->color = { color.x, color.y, color.z, color.a };
    target->TextureCoordinates = { 0.0f, 0.0f };
    target->TextureID = textureID;
    target++;

    target->position = { position.x + size, position.y };
    target->color = { color.x, color.y, color.z, color.a };
    target->TextureCoordinates = { 1.0f, 0.0f };
    target->TextureID = textureID;
    target++;

    target->position = { position.x + size, position.y + size };
    target->color = { color.x, color.y, color.z, color.a };
    target->TextureCoordinates = { 1.0f, 1.0f };
    target->TextureID = textureID;
    target++;

    target->position = { position.x, position.y + size };
    target->color = { color.x, color.y, color.z, color.a };
    target->TextureCoordinates = { 0.0f, 1.0f };
    target->TextureID = textureID;
    target++;

    return target;
}

std::array<Vertex, 4> Shape::createSquare(glm::vec2 position, float size, glm::vec4 color, float textureID)
{
    Vertex v0;
    v0.position = { position.x, position.y };
    v0.color = { color.x, color.y, color.z, color.a };
    v0.TextureCoordinates = { 0.0f, 0.0f };
    v0.TextureID = textureID;

    Vertex v1;
    v1.position = { position.x + size, position.y };
    v1.color = { color.x, color.y, color.z, color.a };
    v1.TextureCoordinates = { 1.0f, 0.0f };
    v1.TextureID = textureID;

    Vertex v2;
    v2.position = { position.x + size, position.y + size };
    v2.color = { color.x, color.y, color.z, color.a };
    v2.TextureCoordinates = { 1.0f, 1.0f };
    v2.TextureID = textureID;

    Vertex v3;
    v3.position = { position.x, position.y + size };
    v3.color = { color.x, color.y, color.z, color.a };
    v3.TextureCoordinates = { 0.0f, 1.0f };
    v3.TextureID = textureID;

    return { v0, v1, v2, v3 };
}