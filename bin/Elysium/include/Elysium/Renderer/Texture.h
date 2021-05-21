#pragma once

#include <glad/glad.h>

#include <glm/glm.hpp>

#include "Elysium/Utility.h"

namespace Elysium
{
    struct TextureData
    {
        friend class Texture;

    private:
        int m_Width, m_Height;

    public:
        unsigned int RendererID = 0;

        bool CoordinatesInverted = false;
        glm::vec2 TextureCoordinates[4] = { {0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f} };

        inline int getHeight() const { return m_Height; }
        inline int getWidth() const { return m_Width; }

        void reflectAroundXAxis();
        void reflectAroundYAxis();
        void subtextureCoordinates(const glm::vec2& coordinates, const glm::vec2& size,
            const glm::vec2& offset = { 0.0f, 0.0f });
    };

    class Texture
    {
    private:
        const char* m_FilePath;
        unsigned char* m_LocalBuffer;
        int m_BPP;

        TextureData m_Data;

    public:
        Texture();
        Texture(const char* filepath, GLenum wrapParams = GL_REPEAT, GLenum filter = GL_LINEAR_MIPMAP_LINEAR);
        virtual ~Texture();

        void Bind(unsigned int slot = 0) const;
        void Unbind() const;

        inline const TextureData& getTextureData() const { return m_Data; }
    };
}