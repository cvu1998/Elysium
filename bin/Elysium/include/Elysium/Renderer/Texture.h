#pragma once

#include <glm/glm.hpp>

#include "Elysium/Utility.h"

struct TextureData
{
    friend class Texture;

private:
    unsigned int m_RendererID;
    int m_Width, m_Height;

    bool m_Default = true;

public:
    bool CoordinatesInverted = false;
    glm::vec2 TextureCoordinates[4] = { {0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f} };

    inline unsigned int getRendererID() const { return m_RendererID; }
    inline int getHeight() const { return m_Height; }
    inline int getWidth() const { return m_Width; }
    inline bool isDefault() const { return m_Default; }

    void reflectAroundXAxis();
    void reflectAroundYAxis();
    void repeatTexture(const glm::vec2& repetition);
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
    Texture(const char* filepath);
    ~Texture();

    void bind(unsigned int slot=0) const;
    void unBind() const;

    inline const TextureData& getTextureData() const { return m_Data; }
};

