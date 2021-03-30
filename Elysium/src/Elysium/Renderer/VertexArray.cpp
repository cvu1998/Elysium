#include "VertexArray.h"

#include "VertexBufferLayout.h"
#include "Elysium/Utility.h"

namespace Elysium
{
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

    void VertexArray::unbind() const
    {
        GL_ASSERT(glBindVertexArray(0));
    }

    void VertexArray::addBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout)
    {
        bind();
        vb.bind();
        const auto& elements = layout.getElements();
#ifndef _WIN64
        unsigned int offset = 0;
#else
        unsigned long long offset = 0;
#endif
        for (unsigned int i = 0; i < elements.size(); ++i)
        {
            const auto& element = elements[i];
            glEnableVertexAttribArray(i);
            glVertexAttribPointer(i, element.count, element.type, element.normalized, layout.getStride(), (const void*)offset);
#ifndef _WIN64
            offset += element.count * VertexBufferElement::getSizeOfType((DataType)element.type);
#else
            offset += (unsigned long long)element.count * VertexBufferElement::getSizeOfType((DataType)element.type);
#endif
        }
    }
}