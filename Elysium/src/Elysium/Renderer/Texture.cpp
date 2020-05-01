#include "Texture.h"

#include "stb_image/stb_image.h"

Texture::Texture()
{
    GL_ASSERT(glGenTextures(1, &m_RendererID));
    GL_ASSERT(glBindTexture(GL_TEXTURE_2D, m_RendererID));

    GL_ASSERT(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GL_ASSERT(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    GL_ASSERT(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GL_ASSERT(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

    unsigned int white = 0xffffffff;
    GL_ASSERT(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, &white));
}

Texture::Texture(const char* filepath) 
    : m_RendererID(0), m_FilePath(filepath), m_LocalBuffer(nullptr), m_Height(0), m_Width(0), m_BPP(0)
{
    stbi_set_flip_vertically_on_load(1);
    m_LocalBuffer = stbi_load(filepath, &m_Width, &m_Height, &m_BPP, 4);

    GLenum internalFormat = 0; 
    GLenum dataFormat = 0;
    if (m_BPP == 4)
    {
        internalFormat = GL_RGBA8;
        dataFormat = GL_RGBA;
    }
    else if (m_BPP == 3)
    {
        internalFormat = GL_RGB8;
        dataFormat = GL_RGB;
    }

    LOG_ASSERT((internalFormat & dataFormat), "Format not supported!");

    GL_ASSERT(glGenTextures(1, &m_RendererID));
    GL_ASSERT(glBindTexture(GL_TEXTURE_2D, m_RendererID));

    GL_ASSERT(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GL_ASSERT(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    GL_ASSERT(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GL_ASSERT(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

    GL_ASSERT(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_LocalBuffer));
    GL_ASSERT(glBindTexture(GL_TEXTURE_2D, 0));

    if (m_LocalBuffer)
        stbi_image_free(m_LocalBuffer);
}

Texture::~Texture()
{
    GL_ASSERT(glDeleteTextures(1, &m_RendererID));
}

void Texture::bind(unsigned int slot) const
{
    GL_ASSERT(glActiveTexture(GL_TEXTURE0 + slot));
    GL_ASSERT(glBindTexture(GL_TEXTURE_2D, m_RendererID));
}

void Texture::unBind() const
{
    GL_ASSERT(glBindTexture(GL_TEXTURE_2D, 0))
}