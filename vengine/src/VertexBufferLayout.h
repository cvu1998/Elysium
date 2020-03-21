#pragma once
#include <vector>

#include "Utility.h"
#include "Shape.h"

struct VertexBufferElement 
{
    unsigned int count;
    unsigned int type;
    unsigned char normalized;

    static unsigned int getSizeOfType(unsigned int type)
    {
        switch (type)
        {
            case GL_FLOAT:          return 4;
            case GL_UNSIGNED_INT:   return 4;
            case GL_UNSIGNED_BYTE:  return 1;
            case 0:                 return sizeof(Vertex);
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
       m_Stride += count * VertexBufferElement::getSizeOfType(GL_FLOAT);
   }

   template<>
   void push<Vertex>(unsigned int count)
   {
       m_Elements.push_back({ count, GL_FLOAT, GL_FALSE });
       m_Stride = VertexBufferElement::getSizeOfType(0);
   }

   template<>
   void push<unsigned int>(unsigned int count)
   {
       m_Elements.push_back({ count, GL_UNSIGNED_INT, GL_FALSE });
       m_Stride += count * VertexBufferElement::getSizeOfType(GL_UNSIGNED_INT);
   }

   template<>
   void push<unsigned char>(unsigned int count)
   {
       m_Elements.push_back({ count, GL_UNSIGNED_BYTE, GL_TRUE });
       m_Stride += count * VertexBufferElement::getSizeOfType(GL_UNSIGNED_BYTE);
   }

   inline std::vector<VertexBufferElement> getElements() const { return m_Elements; }
   inline unsigned int getStride() const { return m_Stride; }
};

