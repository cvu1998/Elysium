#include <Array>

struct Vec2 {
    float x, y;
};

struct Vec3 {
    float x, y, z;
};

struct Vec4 {
    float x, y, z, a;
};

struct Vertex {
    Vec2 position;
    Vec4 color;
    Vec2 textureCoord;
    float textureID;
};

class Shape
{
public:
    static std::array<Vertex, 4> createSquare(Vec2 position, float size, Vec4 color, float textureID);
};