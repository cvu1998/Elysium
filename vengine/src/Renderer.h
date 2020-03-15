#pragma once

#include "IndexBuffer.h"
#include "Shader.h"
#include "VertexArray.h"

class Renderer 
{
public:
    static Renderer& getInstance() {
        static Renderer renderer;
        return renderer;
    }

    void clear() const;
    void draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const;
};

