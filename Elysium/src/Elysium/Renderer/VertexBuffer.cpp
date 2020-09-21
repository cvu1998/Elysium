#include "VertexBuffer.h"

#include <glad/glad.h>

#include "Elysium/Utility.h"

VertexBuffer::VertexBuffer(unsigned int numberofVertices, DataType type)
{
    unsigned int size = VertexBufferElement::getSizeOfType(type);
    glGenBuffers(1, &m_RendererID);
    glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
    glBufferData(GL_ARRAY_BUFFER, size * numberofVertices, nullptr, GL_DYNAMIC_DRAW);
}

VertexBuffer::VertexBuffer(const void* data, unsigned int size) {

    glGenBuffers(1, &m_RendererID);
    glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}

VertexBuffer::~VertexBuffer() {
    GL_ASSERT(glDeleteBuffers(1, &m_RendererID));
}

void VertexBuffer::bind() const {
    glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
}

void VertexBuffer::unBind() const {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}