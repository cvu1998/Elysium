#include "Renderer.h"

#include "Utility.h"

void Renderer::Clear()
{
    glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader, unsigned int count)
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