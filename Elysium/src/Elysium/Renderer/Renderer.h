#pragma once

#include "IndexBuffer.h"
#include "Shader.h"
#include "VertexArray.h"

class Renderer
{
public:
    static void Clear(glm::vec4 color = { 0.0f, 0.0f, 0.0f, 1.0f });
    static void Draw(const VertexArray& va, const IndexBuffer& ib, const Elysium::Shader& shader, unsigned int count=0);
    static void setViewport(unsigned int x, unsigned int y, unsigned int width, unsigned int height);
};

