#include "Texture.h"

#include "stb_image/stb_image.h"

Texture::Texture(const char* filepath) 
    : m_RendererID(0), m_FilePath(filepath), m_LocalBuffer(nullptr), m_Height(0), m_Width(0), m_BPP(0)
{
    stbi_set_flip_vertically_on_load(1);
    m_LocalBuffer = stbi_load(filepath, &m_Width, &m_Height, &m_BPP, 4);

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

void Texture::bind(unsigned int slot /*= 0*/) const
{
    GL_ASSERT(glActiveTexture(GL_TEXTURE0 + slot));
    GL_ASSERT(glBindTexture(GL_TEXTURE_2D, m_RendererID));
}

void Texture::unBind() const
{
    GL_ASSERT(glBindTexture(GL_TEXTURE_2D, 0))
}