#pragma once

#include "IndexBuffer.h"
#include "Shader.h"
#include "VertexArray.h"

class Renderer 
{
public:
    static void clear();
    static void draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader, unsigned int count=0);
};

