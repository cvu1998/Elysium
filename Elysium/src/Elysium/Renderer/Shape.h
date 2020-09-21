#include <glm/glm.hpp>

struct QuadVertex {
    glm::vec2 position;
    glm::vec4 color;
    glm::vec2 TextureCoordinates;
    float TextureID;
};

struct Vertex {
    glm::vec2 position;
    glm::vec4 color;
};