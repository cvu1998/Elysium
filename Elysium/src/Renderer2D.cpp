#include "Renderer2D.h"

#include <array>

static const size_t MaxQuadCount = 10000;
static const size_t MaxVertexCount = MaxQuadCount * 4;
static const size_t MaxIndexCount = MaxQuadCount * 6;
static const size_t MaxTextureCount = 32;

struct Renderer2DData {

    //unsigned int VertexBufferID;
    std::unique_ptr<VertexBuffer> vBuffer;
    std::unique_ptr<IndexBuffer> iBuffer;
    std::unique_ptr<VertexArray> vArray;

    std::unique_ptr<Shader> shader;

    Texture white;
    unsigned int WhiteTextureSlot = 0;

    unsigned int IndexCount = 0;

    Vertex* buffer = nullptr;
    Vertex* BufferPtr = nullptr;

    std::array<unsigned int, MaxTextureCount> TextureSlots;
    unsigned int TextureSlotIndex = 1;

    Renderer2D::Stats RendererStats;

};

/*To Destroy Object on Shutdown*/
static Renderer2DData* s_Data;

void Renderer2D::init()
{
    s_Data = new Renderer2DData;
    s_Data->buffer = new Vertex[MaxVertexCount];

    s_Data->vArray = std::make_unique<VertexArray>();
    s_Data->vArray->bind();

    s_Data->vBuffer = std::make_unique<VertexBuffer>(MaxVertexCount);

    VertexBufferLayout layout;
    layout.push<float>(2);
    layout.push<float>(4);
    layout.push<float>(2);
    layout.push<float>(1);
    s_Data->vArray->addBuffer(*(s_Data->vBuffer), layout);

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

    s_Data->iBuffer = std::make_unique<IndexBuffer>(indices, MaxIndexCount);

    s_Data->TextureSlots[0] = s_Data->white.getRendererID();
    //std::cout << "WhiteTexture:  " << s_Data->white.getRendererID() << std::endl;
    for (size_t i = 1; i < MaxTextureCount; i++)
    {
        s_Data->TextureSlots[i] = 0;
    }

    s_Data->shader = std::make_unique<Shader>("res/shaders/batch_rendering.shader");
    s_Data->shader->bind();

    int sampler[32];
    for (int i = 0; i < 32; i++)
        sampler[i] = i;
    s_Data->shader->setUniform1iv<32>("u_Textures", 32, sampler);
    
}

void Renderer2D::shutdown()
{
    delete[] s_Data->buffer;
    delete s_Data;
}

void Renderer2D::beginBatch()
{
    s_Data->BufferPtr = s_Data->buffer;
}

void Renderer2D::endBatch()
{
    GLsizeiptr size = (uint8_t*)s_Data->BufferPtr - (uint8_t*)s_Data->buffer;
    s_Data->vBuffer->bind();
    GL_ASSERT(glBufferSubData(GL_ARRAY_BUFFER, 0, size, s_Data->buffer));
}

void Renderer2D::flush()
{
    glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
    glm::mat4 proj = glm::ortho(-4.0f, 4.0f, -3.0f, 3.0f, -1.0f, 1.0f);
    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
    glm::mat4 mvp = proj * view * model;
    s_Data->shader->setUniformMat4f("u_MVP", mvp);

    for (unsigned int i = 0; i < s_Data->TextureSlotIndex; i++) {
        GL_ASSERT(glBindTextureUnit(i, s_Data->TextureSlots[i]));
        //std::cout << i << " " << s_Data->TextureSlots[i] << std::endl;
    }
    
    s_Data->vArray->bind();
    GL_ASSERT(glDrawElements(GL_TRIANGLES, s_Data->IndexCount, GL_UNSIGNED_INT, nullptr));
    s_Data->RendererStats.DrawCount++;

    s_Data->IndexCount = 0;
    s_Data->TextureSlotIndex = 1;
}

void Renderer2D::drawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color)
{
    if (s_Data->IndexCount >= MaxIndexCount)
    {
        endBatch();
        flush();
        beginBatch();
    }

    float textureIndex = 0.0f;
    float halfLength = size.x / 2;
    float halfWidth = size.y / 2;

    s_Data->BufferPtr->position = { position.x, position.y };
    s_Data->BufferPtr->color = color;
    s_Data->BufferPtr->TextureCoordinates = { 0.0f, 0.0f };
    s_Data->BufferPtr->TextureID = textureIndex;
    s_Data->BufferPtr++;

    s_Data->BufferPtr->position = { position.x + size.x, position.y };
    s_Data->BufferPtr->color = color;
    s_Data->BufferPtr->TextureCoordinates = { 1.0f, 0.0f };
    s_Data->BufferPtr->TextureID = textureIndex;
    s_Data->BufferPtr++;

    s_Data->BufferPtr->position = { position.x + size.x, position.y + size.y };
    s_Data->BufferPtr->color = color;
    s_Data->BufferPtr->TextureCoordinates = { 1.0f, 1.0f };
    s_Data->BufferPtr->TextureID = textureIndex;
    s_Data->BufferPtr++;

    s_Data->BufferPtr->position = { position.x, position.y + size.y };
    s_Data->BufferPtr->color = color;
    s_Data->BufferPtr->TextureCoordinates = { 0.0f, 1.0f };
    s_Data->BufferPtr->TextureID = textureIndex;
    s_Data->BufferPtr++;

    s_Data->IndexCount += 6;
    s_Data->RendererStats.QuadCount++;
}


void Renderer2D::drawQuad(const glm::vec2& position, const glm::vec2& size, unsigned int textureID)
{
    if (s_Data->IndexCount >= MaxIndexCount || s_Data->TextureSlotIndex > 32)
    {
        endBatch();
        flush();
        beginBatch();
    }

    constexpr glm::vec4 color = { 1.0f, 1.0f, 1.0f, 1.0f };

    float textureIndex = 0.0f;
    for (unsigned int i = 1; i < s_Data->TextureSlotIndex; i++)
    {
        if (s_Data->TextureSlots[i] == textureID)
        {
            textureIndex = float(i);
            break;
        }
    }

    if (textureIndex == 0.0f)
    {
        textureIndex = (float)s_Data->TextureSlotIndex;
        s_Data->TextureSlots[s_Data->TextureSlotIndex] = textureID;
        s_Data->TextureSlotIndex++;
    }

    s_Data->BufferPtr->position = { position.x, position.y };
    s_Data->BufferPtr->color = color;
    s_Data->BufferPtr->TextureCoordinates = { 0.0f, 0.0f };
    s_Data->BufferPtr->TextureID = textureIndex;
    s_Data->BufferPtr++;

    s_Data->BufferPtr->position = { position.x + size.x, position.y };
    s_Data->BufferPtr->color = color;
    s_Data->BufferPtr->TextureCoordinates = { 1.0f, 0.0f };
    s_Data->BufferPtr->TextureID = textureIndex;
    s_Data->BufferPtr++;

    s_Data->BufferPtr->position = { position.x + size.x, position.y + size.y };
    s_Data->BufferPtr->color = color;
    s_Data->BufferPtr->TextureCoordinates = { 1.0f, 1.0f };
    s_Data->BufferPtr->TextureID = textureIndex;
    s_Data->BufferPtr++;

    s_Data->BufferPtr->position = { position.x, position.y + size.y };
    s_Data->BufferPtr->color = color;
    s_Data->BufferPtr->TextureCoordinates = { 0.0f, 1.0f };
    s_Data->BufferPtr->TextureID = textureIndex;
    s_Data->BufferPtr++;

    s_Data->IndexCount += 6;
    s_Data->RendererStats.QuadCount++;
}

void Renderer2D::drawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color, unsigned int textureID)
{
    if (s_Data->IndexCount >= MaxIndexCount || s_Data->TextureSlotIndex > 32)
    {
        endBatch();
        flush();
        beginBatch();
    }

    float textureIndex = 0.0f;
    for (unsigned int i = 1; i < s_Data->TextureSlotIndex; i++)
    {
        if (s_Data->TextureSlots[i] == textureID)
        {
            textureIndex = float(i);
            break;
        }
    }

    if (textureIndex == 0.0f)
    {
        textureIndex = (float)s_Data->TextureSlotIndex;
        s_Data->TextureSlots[s_Data->TextureSlotIndex] = textureID;
        s_Data->TextureSlotIndex++;
    }

    s_Data->BufferPtr->position = { position.x, position.y };
    s_Data->BufferPtr->color = color;
    s_Data->BufferPtr->TextureCoordinates = { 0.0f, 0.0f };
    s_Data->BufferPtr->TextureID = textureIndex;
    s_Data->BufferPtr++;

    s_Data->BufferPtr->position = { position.x + size.x, position.y };
    s_Data->BufferPtr->color = color;
    s_Data->BufferPtr->TextureCoordinates = { 1.0f, 0.0f };
    s_Data->BufferPtr->TextureID = textureIndex;
    s_Data->BufferPtr++;

    s_Data->BufferPtr->position = { position.x + size.x, position.y + size.y };
    s_Data->BufferPtr->color = color;
    s_Data->BufferPtr->TextureCoordinates = { 1.0f, 1.0f };
    s_Data->BufferPtr->TextureID = textureIndex;
    s_Data->BufferPtr++;

    s_Data->BufferPtr->position = { position.x, position.y + size.y };
    s_Data->BufferPtr->color = color;
    s_Data->BufferPtr->TextureCoordinates = { 0.0f, 1.0f };
    s_Data->BufferPtr->TextureID = textureIndex;
    s_Data->BufferPtr++;

    s_Data->IndexCount += 6;
    s_Data->RendererStats.QuadCount++;
}

Renderer2D::Stats& Renderer2D::getStats()
{
    return s_Data->RendererStats;
}

void Renderer2D::resetStats()
{
    memset(&s_Data->RendererStats, 0, sizeof(Stats));
}