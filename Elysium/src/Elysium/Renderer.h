#pragma once

#include "IndexBuffer.h"
#include "Shader.h"
#include "VertexArray.h"

class Renderer
{
public:
    static void Clear();
    static void Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader, unsigned int count=0);
    static void setViewport(unsigned int x, unsigned int y, unsigned int width, unsigned int height);
};

