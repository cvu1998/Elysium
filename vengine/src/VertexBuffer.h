#pragma once

class VertexBuffer {
private:
    unsigned int m_RendererID;

public:
    /*STATIC RENDERING*/
    VertexBuffer(const void* data, unsigned int size);
    /*DYNAMIC BATCH RENDERING*/
    VertexBuffer(unsigned int numberofVertices);
    ~VertexBuffer();

    void bind() const;
    void unBind() const;
};