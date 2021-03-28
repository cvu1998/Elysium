#include <glm/glm.hpp>

struct QuadVertex {
    glm::vec2 Position;
    glm::vec4 Color;
    glm::vec2 TextureCoordinates;
    float TextureID;
};

struct Vertex {
    glm::vec2 Position;
    glm::vec4 Color;
};