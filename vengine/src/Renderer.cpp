#include "Renderer.h"

#include "Utility.h"

void Renderer::clear()
{
    glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader, unsigned int count)
{
    shader.bind();
    va.bind();
    ib.bind();

    if (count == 0)
        count = ib.getCount();

    GL_ASSERT(glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr));
}