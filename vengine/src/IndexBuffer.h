#pragma once

class IndexBuffer {
private:
    unsigned int m_RendererID;
    unsigned int m_Count;

public:
    /*STATIC RENDERING*/
    IndexBuffer(const unsigned int* data, unsigned int count);
    /*DYNAMIC BATCH RENDERING*/
    IndexBuffer(unsigned int count);
    ~IndexBuffer();

    void bind() const;
    void unBind() const;

    inline unsigned int getCount() const { return m_Count; }
};