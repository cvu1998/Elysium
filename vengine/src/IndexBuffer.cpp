#include "IndexBuffer.h"

#include "Utility.h"

IndexBuffer::IndexBuffer(unsigned int count) : m_Count(count) {

    ASSERT(sizeof(unsigned int) == sizeof(GLuint));

    //Allocate indexes buffer
    glGenBuffers(1, &m_RendererID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(GLuint), nullptr, GL_DYNAMIC_DRAW);
}

IndexBuffer::IndexBuffer(const unsigned int* data, unsigned int count) : m_Count(count) {

    ASSERT(sizeof(unsigned int) == sizeof(GLuint));

    //Allocate indexes buffer
    glGenBuffers(1, &m_RendererID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(GLuint), data, GL_STATIC_DRAW);
}

IndexBuffer::~IndexBuffer() {
    GL_ASSERT(glDeleteBuffers(1, &m_RendererID));
}

void IndexBuffer::bind() const {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
}

void IndexBuffer::unBind() const {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}