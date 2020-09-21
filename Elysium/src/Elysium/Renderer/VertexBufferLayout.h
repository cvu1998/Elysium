#pragma once

#include <vector>

#ifdef APIENTRY
#undef APIENTRY
#endif

#include <glad/glad.h>

#include "Elysium/Renderer/Shape.h"
#include "Elysium/Utility.h"

enum class DataType {
    FLOAT = GL_FLOAT,
    UNSIGNED_INT = GL_UNSIGNED_INT,
    UNSIGNED_BYTE = GL_UNSIGNED_BYTE,
    QUAD_VERTEX = 0,
    VERTEX = 1
};

struct VertexBufferElement 
{
    unsigned int count;
    unsigned int type;
    unsigned char normalized;

    static unsigned int getSizeOfType(DataType type)
    {
        switch (type)
        {
        case DataType::FLOAT:          return sizeof(GL_FLOAT);
        case DataType::UNSIGNED_INT:   return sizeof(GL_UNSIGNED_INT);
        case DataType::UNSIGNED_BYTE:  return sizeof(GL_UNSIGNED_BYTE);
        case DataType::QUAD_VERTEX:    return sizeof(QuadVertex);
        case DataType::VERTEX:         return sizeof(Vertex);
        }
        ASSERT(false);
        return 0;
    }
};

class VertexBufferLayout
{
private :
    std::vector<VertexBufferElement> m_Elements;
    unsigned int m_Stride;

public:
    VertexBufferLayout() : m_Stride(0) { }

   template<typename T>
   void push(unsigned int count)
   {
       static_assert(false);
   }

   template<>
   void push<float>(unsigned int count)
   {
       m_Elements.push_back({ count, GL_FLOAT, GL_FALSE });
       m_Stride += count * VertexBufferElement::getSizeOfType(DataType::FLOAT);
   }

   template<>
   void push<QuadVertex>(unsigned int count)
   {
       m_Elements.push_back({ count, GL_FLOAT, GL_FALSE });
       m_Stride = VertexBufferElement::getSizeOfType(DataType::QUAD_VERTEX);
   }

   template<>
   void push<unsigned int>(unsigned int count)
   {
       m_Elements.push_back({ count, GL_UNSIGNED_INT, GL_FALSE });
       m_Stride += count * VertexBufferElement::getSizeOfType(DataType::UNSIGNED_INT);
   }

   template<>
   void push<unsigned char>(unsigned int count)
   {
       m_Elements.push_back({ count, GL_UNSIGNED_BYTE, GL_TRUE });
       m_Stride += count * VertexBufferElement::getSizeOfType(DataType::UNSIGNED_BYTE);
   }

   inline std::vector<VertexBufferElement> getElements() const { return m_Elements; }
   inline unsigned int getStride() const { return m_Stride; }
};

