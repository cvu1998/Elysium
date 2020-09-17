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
        static void beginBatch();
        static void endBatch();
        static void flush();

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

        struct Stats {
            unsigned int DrawCount = 0;
            unsigned int QuadCount = 0;
        };

        static Stats& getStats();
        static void resetStats();
    };
}

