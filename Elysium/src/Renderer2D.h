#pragma once

#include "Renderer.h"
#include "Texture.h"

#include <memory>

class Renderer2D
{
public:
    static void init();
    static void shutdown();

    static void beginBatch();
    static void endBatch();
    static void flush();

    static void drawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color);
    static void drawQuad(const glm::vec2& position, const glm::vec2& size, unsigned int textureID);
    static void drawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color, unsigned int textureID);

    //static void bindTextures(std::vector<Texture>& textures);

    struct Stats {
        unsigned int DrawCount = 0;
        unsigned int QuadCount = 0;
    };

    static Stats& getStats();
    static void resetStats();
};

