#pragma once

#include "Camera.h"
#include "Renderer.h"
#include "Texture.h"

#include <array>
#include <memory>

namespace Elysium
{
    class Renderer2D
    {
    private:
        static void beginQuadBatch();
        static void endQuadBatch();
        static void flushQuads();

        static void beginLineBatch();
        static void endLineBatch();
        static void flushLines();

        static void beginPointBatch();
        static void endPointBatch();
        static void flushPoints();

    public:
        /***ONLY CALL ONCE***/
        static void Init();
        static void Shutdown();
        /***ONLY CALL ONCE***/

        static void beginScene(const Elysium::OrthographicCamera& camera);
        static void endScene();

        // Positions
        // v3----v2
        // |      |
        // |      |
        // v0----v1
        static void drawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color);
        static void drawQuad(const glm::vec2& position, const glm::vec2& size, const TextureData& texture,
            const glm::vec4& color = { 1.0f, 1.0f, 1.0f, 1.0f });

        // ---Rotation in radians---
        static void drawQuadWithRotation(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color);
        static void drawQuadWithRotation(const glm::vec2& position, const glm::vec2& size, float rotation, const TextureData& texture,
            const glm::vec4& color = { 1.0f, 1.0f, 1.0f, 1.0f });

        static void drawLine(const glm::vec2& p0, const glm::vec2& p1,
            const glm::vec4& color = { 1.0f, 1.0f, 1.0f, 1.0f });

        static void drawPoint(const glm::vec2& position,
            const glm::vec4& color = { 1.0f, 1.0f, 1.0f, 1.0f });

        static void setLineWidth(float width);
        static void setPointSize(float size);

        struct Stats {
            unsigned int DrawCount = 0;
            unsigned int QuadCount = 0;
            unsigned int LineCount = 0;
        };

        static Stats& getStats();
        static void resetStats();
    };
}

