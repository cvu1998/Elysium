#pragma once

class IndexBuffer {
private:
    unsigned int m_RendererID;
    unsigned int m_Count;

public:
    /*DYNAMIC BATCH RENDERING*/
    IndexBuffer(unsigned int count);
    /*STATIC RENDERING*/
    IndexBuffer(const unsigned int* data, unsigned int count);
    ~IndexBuffer();

    void bind() const;
    void unbind() const;

    inline unsigned int getCount() const { return m_Count; }
};