#pragma once

#include "Elysium/Renderer/VertexBufferLayout.h"

namespace Elysium
{
    class VertexBuffer
    {
    private:
        unsigned int m_RendererID;

    public:
        /*DYNAMIC BATCH RENDERING*/
        VertexBuffer(unsigned int numberofVertices, DataType type);
        /*STATIC RENDERING*/
        VertexBuffer(const void* data, unsigned int size);
        virtual ~VertexBuffer();

        void bind() const;
        void unbind() const;

        inline unsigned int getRendererID() const { return m_RendererID; };
    };
}