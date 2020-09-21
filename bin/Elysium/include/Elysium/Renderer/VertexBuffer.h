#pragma once

#include "Elysium/Renderer/VertexBufferLayout.h"

class VertexBuffer {
private:
    unsigned int m_RendererID;

public:
    /*DYNAMIC BATCH RENDERING*/
    VertexBuffer(unsigned int numberofVertices, DataType type);
    /*STATIC RENDERING*/
    VertexBuffer(const void* data, unsigned int size);
    ~VertexBuffer();

    void bind() const;
    void unBind() const;

    inline unsigned int getRendererID() const { return m_RendererID; };
};