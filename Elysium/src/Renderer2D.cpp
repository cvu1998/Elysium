#include "Renderer2D.h"

#include <array>

static const size_t MaxQuadCount = 10000;
static const size_t MaxVertexCount = MaxQuadCount * 4;
static const size_t MaxIndexCount = MaxQuadCount * 6;
static const size_t MaxTextureCount = 32;

struct Renderer2DData {

    unsigned int VertexBufferID;
    unsigned int IndexBufferID;
    unsigned int VertexArrayID;

    unsigned int WhiteTexture = 0;
    unsigned int WhiteTextureSlot = 0;

    unsigned int IndexCount = 0;

    Vertex* buffer = nullptr;
    Vertex* BufferPtr = nullptr;

    std::array<unsigned int, MaxTextureCount> TextureSlots;
    unsigned int TextureSlotIndex = 1;

    Renderer2D::Stats RendererStats;

};

static Renderer2DData s_Data;

void Renderer2D::init()
{
    s_Data.buffer = new Vertex[MaxVertexCount];

    glCreateVertexArrays(1, &s_Data.VertexArrayID);
    glBindVertexArray(s_Data.VertexArrayID);

    glGenBuffers(1, &s_Data.VertexBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, s_Data.VertexBufferID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * MaxVertexCount, nullptr, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, position));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, color));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, TextureCoordinates));

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, TextureID));

    unsigned int offset = 0;
    unsigned int indices[MaxIndexCount];
    for (size_t i = 0; i < MaxIndexCount; i += 6)
    {
        indices[i + 0] = 0 + offset;
        indices[i + 1] = 1 + offset;
        indices[i + 2] = 2 + offset;

        indices[i + 3] = 2 + offset;
        indices[i + 4] = 3 + offset;
        indices[i + 5] = 0 + offset;

        offset += 4;
    }

    glGenBuffers(1, &s_Data.IndexBufferID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_Data.IndexBufferID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, MaxIndexCount * sizeof(GLuint), indices, GL_STATIC_DRAW);
    
    GL_ASSERT(glGenTextures(1, &s_Data.WhiteTexture));
    GL_ASSERT(glBindTexture(GL_TEXTURE_2D, s_Data.WhiteTexture));

    GL_ASSERT(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GL_ASSERT(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    GL_ASSERT(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GL_ASSERT(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

    unsigned int white = 0xffffffff;
    GL_ASSERT(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, &white));
    //std::cout << "WhiteTexture:  " << s_Data.WhiteTexture << std::endl;

    s_Data.TextureSlots[0] = s_Data.WhiteTexture;
    for (size_t i = 1; i < MaxTextureCount; i++)
    {
        s_Data.TextureSlots[i] = 0;
    }
    
}

void Renderer2D::shutdown()
{
    GL_ASSERT(glDeleteVertexArrays(1, &s_Data.VertexArrayID));
    GL_ASSERT(glDeleteBuffers(1, &s_Data.VertexBufferID));
    GL_ASSERT(glDeleteBuffers(1, &s_Data.IndexBufferID));

    GL_ASSERT(glDeleteTextures(1, &s_Data.WhiteTexture));
    delete[] s_Data.buffer;
}

void Renderer2D::beginBatch()
{
    s_Data.BufferPtr = s_Data.buffer;
}

void Renderer2D::endBatch()
{
    GLsizeiptr size = (uint8_t*)s_Data.BufferPtr - (uint8_t*)s_Data.buffer;
    GL_ASSERT(glBindBuffer(GL_ARRAY_BUFFER, s_Data.VertexBufferID));
    GL_ASSERT(glBufferSubData(GL_ARRAY_BUFFER, 0, size, s_Data.buffer));
}

void Renderer2D::flush()
{
    for (unsigned int i = 0; i < s_Data.TextureSlotIndex; i++) {
        glBindTextureUnit(i, s_Data.TextureSlots[i]);
        //std::cout << i << " " << s_Data.TextureSlots[i] << std::endl;
    }
    
    GL_ASSERT(glBindVertexArray(s_Data.VertexArrayID));
    GL_ASSERT(glDrawElements(GL_TRIANGLES, s_Data.IndexCount, GL_UNSIGNED_INT, nullptr));
    s_Data.RendererStats.DrawCount++;

    s_Data.IndexCount = 0;
    s_Data.TextureSlotIndex = 1;
}

void Renderer2D::drawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color)
{
    if (s_Data.IndexCount >= MaxIndexCount)
    {
        endBatch();
        flush();
        beginBatch();
    }

    float textureIndex = 0.0f;

    s_Data.BufferPtr->position = { position.x, position.y };
    s_Data.BufferPtr->color = color;
    s_Data.BufferPtr->TextureCoordinates = { 0.0f, 0.0f };
    s_Data.BufferPtr->TextureID = textureIndex;
    s_Data.BufferPtr++;

    s_Data.BufferPtr->position = { position.x + size.x, position.y };
    s_Data.BufferPtr->color = color;
    s_Data.BufferPtr->TextureCoordinates = { 1.0f, 0.0f };
    s_Data.BufferPtr->TextureID = textureIndex;
    s_Data.BufferPtr++;

    s_Data.BufferPtr->position = { position.x + size.x, position.y + size.y };
    s_Data.BufferPtr->color = color;
    s_Data.BufferPtr->TextureCoordinates = { 1.0f, 1.0f };
    s_Data.BufferPtr->TextureID = textureIndex;
    s_Data.BufferPtr++;

    s_Data.BufferPtr->position = { position.x, position.y + size.y };
    s_Data.BufferPtr->color = color;
    s_Data.BufferPtr->TextureCoordinates = { 0.0f, 1.0f };
    s_Data.BufferPtr->TextureID = textureIndex;
    s_Data.BufferPtr++;

    s_Data.IndexCount += 6;
    s_Data.RendererStats.QuadCount++;
}


void Renderer2D::drawQuad(const glm::vec2& position, const glm::vec2& size, unsigned int textureID)
{
    if (s_Data.IndexCount >= MaxIndexCount || s_Data.TextureSlotIndex > 32)
    {
        endBatch();
        flush();
        beginBatch();
    }

    constexpr glm::vec4 color = { 1.0f, 1.0f, 1.0f, 1.0f };

    float textureIndex = 0.0f;
    for (unsigned int i = 1; i < s_Data.TextureSlotIndex; i++)
    {
        if (s_Data.TextureSlots[i] == textureID)
        {
            textureIndex = float(i);
            break;
        }
    }

    if (textureIndex == 0.0f)
    {
        textureIndex = (float)s_Data.TextureSlotIndex;
        s_Data.TextureSlots[s_Data.TextureSlotIndex] = textureID;
        s_Data.TextureSlotIndex++;
    }

    s_Data.BufferPtr->position = { position.x, position.y };
    s_Data.BufferPtr->color = color;
    s_Data.BufferPtr->TextureCoordinates = { 0.0f, 0.0f };
    s_Data.BufferPtr->TextureID = textureIndex;
    s_Data.BufferPtr++;

    s_Data.BufferPtr->position = { position.x + size.x, position.y };
    s_Data.BufferPtr->color = color;
    s_Data.BufferPtr->TextureCoordinates = { 1.0f, 0.0f };
    s_Data.BufferPtr->TextureID = textureIndex;
    s_Data.BufferPtr++;

    s_Data.BufferPtr->position = { position.x + size.x, position.y + size.y };
    s_Data.BufferPtr->color = color;
    s_Data.BufferPtr->TextureCoordinates = { 1.0f, 1.0f };
    s_Data.BufferPtr->TextureID = textureIndex;
    s_Data.BufferPtr++;

    s_Data.BufferPtr->position = { position.x, position.y + size.y };
    s_Data.BufferPtr->color = color;
    s_Data.BufferPtr->TextureCoordinates = { 0.0f, 1.0f };
    s_Data.BufferPtr->TextureID = textureIndex;
    s_Data.BufferPtr++;

    s_Data.IndexCount += 6;
    s_Data.RendererStats.QuadCount++;
}

void Renderer2D::drawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color, unsigned int textureID)
{
    if (s_Data.IndexCount >= MaxIndexCount || s_Data.TextureSlotIndex > 32)
    {
        endBatch();
        flush();
        beginBatch();
    }

    float textureIndex = 0.0f;
    for (unsigned int i = 1; i < s_Data.TextureSlotIndex; i++)
    {
        if (s_Data.TextureSlots[i] == textureID)
        {
            textureIndex = float(i);
            break;
        }
    }

    if (textureIndex == 0.0f)
    {
        textureIndex = (float)s_Data.TextureSlotIndex;
        s_Data.TextureSlots[s_Data.TextureSlotIndex] = textureID;
        s_Data.TextureSlotIndex++;
    }

    s_Data.BufferPtr->position = { position.x, position.y };
    s_Data.BufferPtr->color = color;
    s_Data.BufferPtr->TextureCoordinates = { 0.0f, 0.0f };
    s_Data.BufferPtr->TextureID = textureIndex;
    s_Data.BufferPtr++;

    s_Data.BufferPtr->position = { position.x + size.x, position.y };
    s_Data.BufferPtr->color = color;
    s_Data.BufferPtr->TextureCoordinates = { 1.0f, 0.0f };
    s_Data.BufferPtr->TextureID = textureIndex;
    s_Data.BufferPtr++;

    s_Data.BufferPtr->position = { position.x + size.x, position.y + size.y };
    s_Data.BufferPtr->color = color;
    s_Data.BufferPtr->TextureCoordinates = { 1.0f, 1.0f };
    s_Data.BufferPtr->TextureID = textureIndex;
    s_Data.BufferPtr++;

    s_Data.BufferPtr->position = { position.x, position.y + size.y };
    s_Data.BufferPtr->color = color;
    s_Data.BufferPtr->TextureCoordinates = { 0.0f, 1.0f };
    s_Data.BufferPtr->TextureID = textureIndex;
    s_Data.BufferPtr++;

    s_Data.IndexCount += 6;
    s_Data.RendererStats.QuadCount++;
}

Renderer2D::Stats& Renderer2D::getStats()
{
    return s_Data.RendererStats;
}

void Renderer2D::resetStats()
{
    memset(&s_Data.RendererStats, 0, sizeof(Stats));
}