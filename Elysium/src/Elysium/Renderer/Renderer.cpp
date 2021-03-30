#include "Renderer.h"

namespace Elysium
{
    void Renderer::Clear(glm::vec4 color)
    {
        GL_ASSERT(glClearColor(color.r, color.g, color.b, color.a));
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void Renderer::Draw(const VertexArray& va, const IndexBuffer& ib, const Elysium::Shader& shader, unsigned int count)
    {
        shader.bind();
        va.bind();
        ib.bind();

        if (count == 0)
            count = ib.getCount();

        GL_ASSERT(glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr));
    }

    void Renderer::setViewport(unsigned int x, unsigned int y, unsigned int width, unsigned int height)
    {
        GL_ASSERT(glViewport(x, y, width, height));
    }
}