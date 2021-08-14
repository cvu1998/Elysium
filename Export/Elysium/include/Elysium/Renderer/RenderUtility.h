#pragma once

#include "IndexBuffer.h"
#include "Shader.h"
#include "VertexArray.h"

namespace Elysium
{
    namespace Render
    {
        void Clear();
        void ClearColor(glm::vec4 color = { 0.0f, 0.0f, 0.0f, 1.0f });
        void Draw(const VertexArray& va, const IndexBuffer& ib, const Elysium::Shader& shader, unsigned int count = 0);
        void setViewport(unsigned int x, unsigned int y, unsigned int width, unsigned int height);
    }
}