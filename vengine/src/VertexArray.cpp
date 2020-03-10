#include "VertexArray.h"

#include "VertexBufferLayout.h"
#include "Utility.h"

VertexArray::VertexArray()
{
    GL_ASSERT(glGenVertexArrays(1, &m_RendererID));
}

VertexArray::~VertexArray()
{
    GL_ASSERT(glDeleteVertexArrays(1, &m_RendererID));
}

void VertexArray::bind() const
{
    GL_ASSERT(glBindVertexArray(m_RendererID));
}

void VertexArray::unBind() const
{
    GL_ASSERT(glBindVertexArray(0));
}

void VertexArray::addBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout)
{
    bind();
    vb.bind();
    const auto& elements = layout.getElements();
    unsigned int offset = 0;
    for (unsigned int i = 0; i < elements.size(); ++i) {
        const auto& element = elements[i];
        glEnableVertexAttribArray(i);
        glVertexAttribPointer(i, element.count, element.type, element.normalized, layout.getStride(), (const void*)offset);
        offset += element.count * VertexBufferElement::getSizeOfType(element.type);
    }
}