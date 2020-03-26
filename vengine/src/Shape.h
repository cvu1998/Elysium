#include <Array>
#include <glm/glm.hpp>

struct Vertex {
    glm::vec2 position;
    glm::vec4 color;
    glm::vec2 TextureCoordinates;
    float TextureID;
};

class Shape
{

public:
    static Vertex* createSquare(Vertex* target, glm::vec2 position, float size, glm::vec4 color, float textureID);
    static std::array<Vertex, 4> createSquare(glm::vec2 position, float size, glm::vec4 color, float textureID);
};