#include "Ground.h"

Ground::Ground(const char* name, const glm::vec2& initialPosition, const glm::vec2& size) : Elysium::StaticObject(name, initialPosition, size)
{
}

void Ground::Draw()
{
    for (float position = Position.x - (Size.x / 2.0f) + 1.0f; position < Size.x / 2.0f; position += 2.0f)
    {
        Elysium::Renderer2D::drawQuad({ position, Position.y }, { 2.0f, 2.0f }, TextureData, Color);
    }
}