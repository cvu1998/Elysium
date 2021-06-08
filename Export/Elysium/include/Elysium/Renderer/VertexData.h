#pragma once

#include <glm/glm.hpp>

namespace Elysium
{
    struct QuadVertex
    {
        glm::vec2 Position;
        glm::vec4 Color;
        glm::vec2 TextureCoordinates;
        glm::vec2 TilingFactor;
        float TextureID;
    };

    struct Vertex
    {
        glm::vec3 Position;
        glm::vec4 Color;
    };
}