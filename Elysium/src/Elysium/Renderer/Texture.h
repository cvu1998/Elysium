#pragma once

#include <string>

#include <glm/glm.hpp>

#include "Elysium/Utility.h"

class Texture
{
private:
    unsigned int m_RendererID;
    const char* m_FilePath;
    unsigned char* m_LocalBuffer;
    int m_Width, m_Height, m_BPP;

    bool m_CoordinatesInverted = false;
    glm::vec2 m_TextureCoordinates[4] = { {0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f} };

public:
    Texture();
    Texture(const char* filepath);
    ~Texture();

    void bind(unsigned int slot=0) const;
    void unBind() const;

    inline unsigned int getRendererID() const { return m_RendererID; }
    inline int getHeight() const { return m_Height; }
    inline int getWidth() const { return m_Width; }

    inline const glm::vec2* getTextureCoordinates() const { return m_TextureCoordinates; }

    void reflectAroundYAxis();
    void subtextureCoordinates(const glm::vec2& coordinates, const glm::vec2& size);
};

