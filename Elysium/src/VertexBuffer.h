#pragma once

class VertexBuffer {
private:
    unsigned int m_RendererID;

public:
    /*DYNAMIC BATCH RENDERING*/
    VertexBuffer(unsigned int numberofVertices);
    /*STATIC RENDERING*/
    VertexBuffer(const void* data, unsigned int size);
    ~VertexBuffer();

    void bind() const;
    void unBind() const;

    inline unsigned int getRendererID() const { return m_RendererID; };
};