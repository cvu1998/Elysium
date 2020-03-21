#include "VertexBuffer.h"

#include "Shape.h"
#include "Utility.h"

VertexBuffer::VertexBuffer(const void* data, unsigned int size) {
    //Allocate vertex buffer
    glGenBuffers(1, &m_RendererID);
    glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}

VertexBuffer::VertexBuffer(unsigned int numberofVertices)
{
    glGenBuffers(1, &m_RendererID);
    glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * numberofVertices, nullptr, GL_DYNAMIC_DRAW);
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