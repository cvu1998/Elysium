#include "Texture.h"

#include "stb_image/stb_image.h"

Texture::Texture()
    : m_FilePath(nullptr), m_LocalBuffer(nullptr), m_BPP(0)
{
    m_Data.m_Default = true;

    GL_ASSERT(glGenTextures(1, &m_Data.m_RendererID));
    GL_ASSERT(glBindTexture(GL_TEXTURE_2D, m_Data.m_RendererID));

    GL_ASSERT(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GL_ASSERT(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
    GL_ASSERT(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
    GL_ASSERT(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));

    unsigned int white = 0xffffffff;
    GL_ASSERT(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, &white));
}

Texture::Texture(const char* filepath)
    : m_FilePath(filepath), m_LocalBuffer(nullptr), m_BPP(0)
{
    m_Data.m_Default = false;

    stbi_set_flip_vertically_on_load(1);
    m_LocalBuffer = stbi_load(filepath, &m_Data.m_Width, &m_Data.m_Height, &m_BPP, 4);

    GL_ASSERT(glGenTextures(1, &m_Data.m_RendererID));
    GL_ASSERT(glBindTexture(GL_TEXTURE_2D, m_Data.m_RendererID));

    GL_ASSERT(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GL_ASSERT(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
    GL_ASSERT(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
    GL_ASSERT(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));

    GL_ASSERT(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Data.m_Width, m_Data.m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_LocalBuffer));
    GL_ASSERT(glBindTexture(GL_TEXTURE_2D, 0));

    if (m_LocalBuffer)
        stbi_image_free(m_LocalBuffer);
}

Texture::~Texture()
{
    GL_ASSERT(glDeleteTextures(1, &m_Data.m_RendererID));
}

void Texture::bind(unsigned int slot) const
{
    GL_ASSERT(glActiveTexture(GL_TEXTURE0 + slot));
    GL_ASSERT(glBindTexture(GL_TEXTURE_2D, m_Data.m_RendererID));
}

void Texture::unBind() const
{
    GL_ASSERT(glBindTexture(GL_TEXTURE_2D, 0))
}

void TextureData::reflectAroundYAxis()
{
    glm::vec2 textureCoordinates[4] = { TextureCoordinates[0],  TextureCoordinates[1],  TextureCoordinates[2],  TextureCoordinates[3] };
    TextureCoordinates[0] = textureCoordinates[1];
    TextureCoordinates[1] = textureCoordinates[0];
    TextureCoordinates[2] = textureCoordinates[3];
    TextureCoordinates[3] = textureCoordinates[2];
    CoordinatesInverted = !CoordinatesInverted;
}

void TextureData::repeatTexture(const glm::vec2& repetition)
{
    TextureCoordinates[0] = { 0.0f, 0.0f };
    TextureCoordinates[1] = { repetition.x, 0.0f };
    TextureCoordinates[2] = { repetition.x, repetition.y };
    TextureCoordinates[3] = { 0.0f, repetition.y };
}

void TextureData::subtextureCoordinates(const glm::vec2& coordinates, const glm::vec2& size)
{
    glm::vec2 minimum = { (coordinates.x * size.x) / m_Width, (coordinates.y * size.y) / m_Height };
    glm::vec2 maximum = { ((coordinates.x + 1) * size.x) / m_Width, ((coordinates.y + 1) * size.y) / m_Height };
    if (!CoordinatesInverted)
    {
        TextureCoordinates[0] = { minimum.x, minimum.y };
        TextureCoordinates[1] = { maximum.x, minimum.y };
        TextureCoordinates[2] = { maximum.x, maximum.y };
        TextureCoordinates[3] = { minimum.x, maximum.y };
    }
    else
    {
        TextureCoordinates[0] = { maximum.x, minimum.y };
        TextureCoordinates[1] = { minimum.x, minimum.y };
        TextureCoordinates[2] = { minimum.x, maximum.y };
        TextureCoordinates[3] = { maximum.x, maximum.y };
    }
}
