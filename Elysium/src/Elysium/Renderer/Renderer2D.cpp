#include "Renderer2D.h"

#include <array>

#include "Elysium/Math.h"

static const size_t MaxQuadCount = 10000;
static const size_t MaxQuadVertexCount = MaxQuadCount * 4;
static const size_t MaxQuadIndexCount = MaxQuadCount * 6;
static const size_t MaxTextureCount = 32;

static const uint32_t MaxLineCount = 20000;
static const uint32_t MaxLineVertexCount = MaxLineCount * 2;
static const uint32_t MaxLineIndexCount = MaxLineCount * 2;

static const uint32_t MaxPointCount = 40000;

namespace Elysium
{
    struct Renderer2DData
    {
        // -----------Quads---------- //

        std::unique_ptr<VertexBuffer> QuadVertexBuffer;
        std::unique_ptr<IndexBuffer> QuadIndexBuffer;
        std::unique_ptr<VertexArray> QuadVertexArray;

        std::unique_ptr<Shader> QuadShader;

        unsigned int QuadIndexCount = 0;

        QuadVertex* QuadBuffer = nullptr;
        QuadVertex* QuadBufferPtr = nullptr;

        Texture White;
        std::array<unsigned int, MaxTextureCount> TextureSlots;
        unsigned int TextureSlotIndex = 1;

        glm::vec2 PositionSign[4] = { { -1.0f, -1.0f }, { 1.0f, -1.0f }, { 1.0f,  1.0f }, {-1.0f, 1.0f } };
        glm::vec2 TextureCoordinates[4] = { { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 1.0f,  1.0f }, { 0.0f, 1.0f } };
        glm::vec2 QuadVertexPositions[4] = { {-0.5, -0.5 }, { 0.5, -0.5 }, { 0.5,  0.5 }, {-0.5,  0.5 } };

        // -----------Lines---------- //

        std::unique_ptr<VertexBuffer> LineVertexBuffer;
        std::unique_ptr<IndexBuffer> LineIndexBuffer;
        std::unique_ptr<VertexArray> LineVertexArray;

        std::unique_ptr<Shader> LineShader;

        unsigned int LineIndexCount = 0;

        Vertex* LineBuffer = nullptr;
        Vertex* LineBufferPtr = nullptr;

        // -----------Points---------- //

        std::unique_ptr<VertexBuffer> PointVertexBuffer;
        std::unique_ptr<VertexArray> PointVertexArray;

        std::unique_ptr<Shader> PointShader;

        unsigned int PointIndexCount = 0;

        Vertex* PointBuffer = nullptr;
        Vertex* PointBufferPtr = nullptr;

        glm::mat4 CameraViewProj;

        Renderer2D::Stats RendererStats;
    };

    /*To Destroy Object on Shutdown*/
    static Renderer2DData* s_Data;

    void Renderer2D::Init()
    {
        s_Data = new Renderer2DData;

        // -----------Quads---------- //

        s_Data->QuadBuffer = new QuadVertex[MaxQuadVertexCount];

        s_Data->QuadVertexArray = std::make_unique<VertexArray>();
        s_Data->QuadVertexArray->bind();

        s_Data->QuadVertexBuffer = std::make_unique<VertexBuffer>((unsigned int)MaxQuadVertexCount, DataType::QUAD_VERTEX);

        VertexBufferLayout quadLayout;
        quadLayout.push<float>(2);
        quadLayout.push<float>(4);
        quadLayout.push<float>(2);
        quadLayout.push<float>(1);
        s_Data->QuadVertexArray->addBuffer(*(s_Data->QuadVertexBuffer), quadLayout);

        unsigned int offset = 0;
        unsigned int quadIndices[MaxQuadIndexCount];
        for (size_t i = 0; i < MaxQuadIndexCount; i += 6)
        {
            quadIndices[i + 0] = 0 + offset;
            quadIndices[i + 1] = 1 + offset;
            quadIndices[i + 2] = 2 + offset;

            quadIndices[i + 3] = 2 + offset;
            quadIndices[i + 4] = 3 + offset;
            quadIndices[i + 5] = 0 + offset;

            offset += 4;
        }

        s_Data->QuadIndexBuffer = std::make_unique<IndexBuffer>(quadIndices, (unsigned int)MaxQuadIndexCount);

        GL_ASSERT(glEnable(GL_BLEND));
        GL_ASSERT(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

        s_Data->TextureSlots[0] = s_Data->White.getTextureData().getRendererID();
        for (size_t i = 1; i < MaxTextureCount; i++)
        {
            s_Data->TextureSlots[i] = 0;
        }

        s_Data->QuadShader = std::make_unique<Shader>("res/shaders/batch_rendering.shader");
        s_Data->QuadShader->bind();

        int sampler[32];
        for (int i = 0; i < 32; i++)
            sampler[i] = i;
        s_Data->QuadShader->setUniform1iv("u_Textures", 32, sampler);

        // -----------Lines---------- //

        s_Data->LineBuffer = new Vertex[MaxLineVertexCount];

        s_Data->LineVertexArray = std::make_unique<VertexArray>();
        s_Data->LineVertexArray->bind();

        s_Data->LineVertexBuffer = std::make_unique<VertexBuffer>((unsigned int)MaxLineVertexCount, DataType::VERTEX);

        VertexBufferLayout lineLayout;
        lineLayout.push<float>(2);
        lineLayout.push<float>(4);
        s_Data->LineVertexArray->addBuffer(*(s_Data->LineVertexBuffer), lineLayout);

        unsigned int lineIndices[MaxLineIndexCount];
        for (size_t i = 0; i < MaxLineIndexCount; i++)
            lineIndices[i] = (unsigned int)i;

        s_Data->LineIndexBuffer = std::make_unique<IndexBuffer>(lineIndices, (unsigned int)MaxLineIndexCount);

        s_Data->LineShader = std::make_unique<Shader>("res/shaders/vertex_primitive.shader");

        // -----------Points---------- //

        s_Data->PointBuffer = new Vertex[MaxPointCount];

        s_Data->PointVertexArray = std::make_unique<VertexArray>();
        s_Data->PointVertexArray->bind();

        s_Data->PointVertexBuffer = std::make_unique<VertexBuffer>((unsigned int)MaxPointCount, DataType::VERTEX);

        VertexBufferLayout pointLayout;
        pointLayout.push<float>(2);
        pointLayout.push<float>(4);
        s_Data->PointVertexArray->addBuffer(*(s_Data->PointVertexBuffer), pointLayout);

        s_Data->PointShader = std::make_unique<Shader>("res/shaders/vertex_primitive.shader");
    }

    void Renderer2D::Shutdown()
    {
        delete[] s_Data->QuadBuffer;
        delete[] s_Data->LineBuffer;
        delete[] s_Data->PointBuffer;
        delete s_Data;
    }

    void Renderer2D::beginScene(const Elysium::OrthographicCamera& camera)
    {
        s_Data->CameraViewProj = camera.getViewProjectionMatrix();

        Renderer2D::beginQuadBatch();
        Renderer2D::beginLineBatch();
        Renderer2D::beginPointBatch();
    }

    void Renderer2D::endScene()
    {
        if (s_Data->PointBufferPtr != s_Data->PointBuffer)
        {
            Renderer2D::endPointBatch();
            Renderer2D::flushPoints();
        }

        if (s_Data->LineBufferPtr != s_Data->LineBuffer)
        {
            Renderer2D::endLineBatch();
            Renderer2D::flushLines();
        }

        if (s_Data->QuadBufferPtr != s_Data->QuadBuffer)
        {
            Renderer2D::endQuadBatch();
            Renderer2D::flushQuads();
        }
    }

    void Renderer2D::beginQuadBatch()
    {
        s_Data->QuadBufferPtr = s_Data->QuadBuffer;
    }

    void Renderer2D::endQuadBatch()
    {
        GLsizeiptr size = (uint8_t*)s_Data->QuadBufferPtr - (uint8_t*)s_Data->QuadBuffer;
        s_Data->QuadVertexBuffer->bind();
        GL_ASSERT(glBufferSubData(GL_ARRAY_BUFFER, 0, size, s_Data->QuadBuffer));
    }

    void Renderer2D::flushQuads()
    {
        s_Data->QuadShader->bind();
        s_Data->QuadShader->setUniformMat4f("u_ViewProjection", s_Data->CameraViewProj);

        for (unsigned int i = 0; i < s_Data->TextureSlotIndex; i++) {
            GL_ASSERT(glActiveTexture(GL_TEXTURE0 + i));
            GL_ASSERT(glBindTexture(GL_TEXTURE_2D, s_Data->TextureSlots[i]));
        }

        s_Data->QuadVertexArray->bind();
        s_Data->QuadIndexBuffer->bind();
        GL_ASSERT(glDrawElements(GL_TRIANGLES, s_Data->QuadIndexCount, GL_UNSIGNED_INT, nullptr));
        s_Data->RendererStats.DrawCount++;

        s_Data->QuadIndexCount = 0;
        s_Data->TextureSlotIndex = 1;
    }

    void Renderer2D::beginLineBatch()
    {
        s_Data->LineBufferPtr = s_Data->LineBuffer;
    }

    void Renderer2D::endLineBatch()
    {
        GLsizeiptr size = (uint8_t*)s_Data->LineBufferPtr - (uint8_t*)s_Data->LineBuffer;
        s_Data->LineVertexBuffer->bind();
        GL_ASSERT(glBufferSubData(GL_ARRAY_BUFFER, 0, size, s_Data->LineBuffer));
    }

    void Renderer2D::flushLines()
    {
        s_Data->LineShader->bind();
        s_Data->LineShader->setUniformMat4f("u_ViewProjection", s_Data->CameraViewProj);

        s_Data->LineVertexArray->bind();
        s_Data->LineIndexBuffer->bind();
        GL_ASSERT(glDrawElements(GL_LINES, s_Data->LineIndexCount, GL_UNSIGNED_INT, nullptr));
        s_Data->RendererStats.DrawCount++;

        s_Data->LineIndexCount = 0;
    }

    void Renderer2D::beginPointBatch()
    {
        s_Data->PointBufferPtr = s_Data->PointBuffer;
    }

    void Renderer2D::endPointBatch()
    {
        GLsizeiptr size = (uint8_t*)s_Data->PointBufferPtr - (uint8_t*)s_Data->PointBuffer;
        s_Data->PointVertexBuffer->bind();
        GL_ASSERT(glBufferSubData(GL_ARRAY_BUFFER, 0, size, s_Data->PointBuffer));
    }

    void Renderer2D::flushPoints()
    {
        s_Data->PointShader->bind();
        s_Data->PointShader->setUniformMat4f("u_ViewProjection", s_Data->CameraViewProj);

        s_Data->PointVertexArray->bind();
        GL_ASSERT(glDrawArrays(GL_POINTS, 0, s_Data->PointIndexCount));
        s_Data->RendererStats.DrawCount++;

        s_Data->PointIndexCount = 0;
    }

    // -----------Draw Quad---------- //

    void Renderer2D::drawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color)
    {
        if (s_Data->QuadIndexCount >= MaxQuadIndexCount)
        {
            endQuadBatch();
            flushQuads();
            beginQuadBatch();
        }

        float textureIndex = 0.0f;
        float halfLength = size.x / 2;
        float halfWidth = size.y / 2;

        for (size_t i = 0; i < 4; i++)
        {

            s_Data->QuadBufferPtr->position = { position.x + halfLength * s_Data->PositionSign[i].x,
                position.y + halfWidth * s_Data->PositionSign[i].y };

            s_Data->QuadBufferPtr->color = color;
            s_Data->QuadBufferPtr->TextureCoordinates = s_Data->TextureCoordinates[i];
            s_Data->QuadBufferPtr->TextureID = textureIndex;
            s_Data->QuadBufferPtr++;
        }

        s_Data->QuadIndexCount += 6;
        s_Data->RendererStats.QuadCount++;
    }

    void Renderer2D::drawQuad(const glm::vec2& position, const glm::vec2& size, const TextureData& texture, const glm::vec4& color)
    {
        if (s_Data->QuadIndexCount >= MaxQuadIndexCount || s_Data->TextureSlotIndex > 32)
        {
            endQuadBatch();
            flushQuads();
            beginQuadBatch();
        }

        unsigned int textureID = texture.getRendererID();
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

        float halfLength = size.x / 2;
        float halfWidth = size.y / 2;

        for (size_t i = 0; i < 4; i++)
        {
            s_Data->QuadBufferPtr->position = { position.x + halfLength * s_Data->PositionSign[i].x,
                position.y + halfWidth * s_Data->PositionSign[i].y };

            s_Data->QuadBufferPtr->color = color;
            s_Data->QuadBufferPtr->TextureCoordinates = texture.TextureCoordinates[i];
            s_Data->QuadBufferPtr->TextureID = textureIndex;
            s_Data->QuadBufferPtr++;
        }

        s_Data->QuadIndexCount += 6;
        s_Data->RendererStats.QuadCount++;
    }

    // -----------Draw Quad With Rotation---------- //

    void Renderer2D::drawQuadWithRotation(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color)
    {
        if (s_Data->QuadIndexCount >= MaxQuadIndexCount)
        {
            endQuadBatch();
            flushQuads();
            beginQuadBatch();
        }

        float textureIndex = 0.0f;

        Complex transform(cos(rotation), sin(rotation));
        Complex translation(position.x, position.y);

        for (size_t i = 0; i < 4; i++)
        {
            s_Data->QuadBufferPtr->position = (glm::vec2) (Complex::scaleXY(Complex(s_Data->QuadVertexPositions[i].x, s_Data->QuadVertexPositions[i].y), size.x, size.y)
                * transform
                + translation);

            s_Data->QuadBufferPtr->color = color;
            s_Data->QuadBufferPtr->TextureCoordinates = s_Data->TextureCoordinates[i];
            s_Data->QuadBufferPtr->TextureID = textureIndex;
            s_Data->QuadBufferPtr++;
        }

        s_Data->QuadIndexCount += 6;
        s_Data->RendererStats.QuadCount++;
    }

    void Renderer2D::drawQuadWithRotation(const glm::vec2& position, const glm::vec2& size, float rotation, const TextureData& texture, const glm::vec4& color)
    {
        if (s_Data->QuadIndexCount >= MaxQuadIndexCount || s_Data->TextureSlotIndex > 32)
        {
            endQuadBatch();
            flushQuads();
            beginQuadBatch();
        }

        unsigned int textureID = texture.getRendererID();
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

        Complex transform(cos(rotation), sin(rotation));
        Complex translation(position.x, position.y);

        for (size_t i = 0; i < 4; i++)
        {
            s_Data->QuadBufferPtr->position = (glm::vec2) (Complex::scaleXY(Complex(s_Data->QuadVertexPositions[i].x, s_Data->QuadVertexPositions[i].y), size.x, size.y)
                * transform
                + translation);

            s_Data->QuadBufferPtr->color = color;
            s_Data->QuadBufferPtr->TextureCoordinates = texture.TextureCoordinates[i];
            s_Data->QuadBufferPtr->TextureID = textureIndex;
            s_Data->QuadBufferPtr++;
        }

        s_Data->QuadIndexCount += 6;
        s_Data->RendererStats.QuadCount++;
    }

    void Renderer2D::drawLine(const glm::vec2& p0, const glm::vec2& p1, const glm::vec4& color)
    {
        if (s_Data->LineIndexCount >= MaxLineIndexCount)
        {
            endLineBatch();
            flushLines();
            beginLineBatch();
        }

        s_Data->LineBufferPtr->position = p0;
        s_Data->LineBufferPtr->color = color;
        s_Data->LineBufferPtr++;

        s_Data->LineBufferPtr->position = p1;
        s_Data->LineBufferPtr->color = color;
        s_Data->LineBufferPtr++;

        s_Data->LineIndexCount += 2;

        s_Data->RendererStats.LineCount++;
    }

    void Renderer2D::drawPoint(const glm::vec2& position, const glm::vec4& color)
    {
        if (s_Data->PointIndexCount >= MaxPointCount)
        {
            endPointBatch();
            flushPoints();
            beginPointBatch();
        }

        s_Data->PointBufferPtr->position = position;
        s_Data->PointBufferPtr->color = color;
        s_Data->PointBufferPtr++;

        s_Data->PointIndexCount++;
    }

    void Renderer2D::setLineWidth(float width)
    {
        GL_ASSERT(glLineWidth(width));
    }

    void Renderer2D::setPointSize(float size)
    {
        GL_ASSERT(glPointSize(size));
    }

    Renderer2D::Stats& Renderer2D::getStats()
    {
        return s_Data->RendererStats;
    }

    void Renderer2D::resetStats()
    {
        memset(&s_Data->RendererStats, 0, sizeof(Stats));
    }
}