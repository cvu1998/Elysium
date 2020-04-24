#pragma once

#include "Renderer.h"
#include "Texture.h"

#include <memory>

class Renderer2D
{
public:
    static void Init();
    static void Shutdown();

    static void beginScene();
    static void endScene();

    static void drawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color);
    static void drawQuad(const glm::vec2& position, const glm::vec2& size, unsigned int textureID, 
        const glm::vec4& color={ 1.0f, 1.0f, 1.0f, 1.0f });

    static void drawQuadWithRotation(const glm::vec2& position, const glm::vec2& size, float rotation, const glm::vec4& color);
    static void drawQuadWithRotation(const glm::vec2& position, const glm::vec2& size, float rotation, unsigned int textureID,
        const glm::vec4& color = { 1.0f, 1.0f, 1.0f, 1.0f });

    struct Stats {
        unsigned int DrawCount = 0;
        unsigned int QuadCount = 0;
    };

    static Stats& getStats();

private:
    static void beginBatch();
    static void endBatch();
    static void flush();

    static void resetStats();
};

