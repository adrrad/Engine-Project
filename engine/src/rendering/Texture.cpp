#include "rendering/Texture.hpp"

#include "rendering/Debugging.hpp"

#include <glad/glad.h>

namespace Engine::Rendering
{

void Texture::UploadTexture()
{
    UPDATE_CALLINFO();
    glGenTextures(1, &m_id);
    glBindTexture(GL_TEXTURE_2D, m_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    int format = m_channels == 1 ? GL_DEPTH_COMPONENT : GL_RGB;
    if(m_channels == 4) format = GL_RGBA;
    UPDATE_CALLINFO();
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        format,
        m_width,
        m_height,
        0,
        format,
        GL_UNSIGNED_BYTE,
        d);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    m_isGLValid = true;
    
}

void Texture::CreateCubemap(Texture* right, Texture* left, Texture* top, Texture* bot, Texture* back, Texture* front)
{
    Texture* textures[] = {right, left, top, bot, front, back};
    glGenTextures(1, &m_id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_id);
    
    for(uint32_t texIndex = 0; texIndex < 6; texIndex++)
    {
        Texture* tex = textures[texIndex];
        glTexImage2D(
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + texIndex, 
            0, 
            GL_RGB, 
            tex->GetWidth(), 
            tex->GetHeight(), 
            0, 
            GL_RGB, 
            GL_UNSIGNED_BYTE, 
            tex->GetData());
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

Texture::Texture(ImageData* data)
{
    m_width = data->Width;
    m_height = data->Height;
    m_channels = data->NumChannels;
    d = data->Pixels.Data();
    m_target = GL_TEXTURE_2D;
    UploadTexture();
}

Texture::Texture(uint32_t width, uint32_t height, uint32_t channels, unsigned char* data)
{
    m_width = width;
    m_height = height;
    m_channels = channels;
    d = data;
    m_target = GL_TEXTURE_2D;
}

Texture::Texture(uint32_t width, uint32_t height, uint32_t channels, unsigned char* data, uint32_t glTarget) : Texture(width, height, channels, data)
{
    m_target = glTarget;
    UploadTexture();
}

Texture::Texture(Texture* right, Texture* left, Texture* top, Texture* bot, Texture* back, Texture* front)
{
    m_target = GL_TEXTURE_CUBE_MAP;
    CreateCubemap(right, left, top, bot, back, front);
}

Texture::Texture(Assets::ImageAsset* right, Assets::ImageAsset* left, Assets::ImageAsset* top, 
    Assets::ImageAsset* bot, Assets::ImageAsset* back, Assets::ImageAsset* front)
{
    m_target = GL_TEXTURE_CUBE_MAP;
    CreateCubemap(new Texture(right->GetImageData()), new Texture(left->GetImageData()), new Texture(top->GetImageData()), 
        new Texture(bot->GetImageData()), new Texture(back->GetImageData()), new Texture(front->GetImageData()));
}


Texture::Texture(Assets::CubemapAsset* cubemap)
{
    
}

Texture::Texture(TextureTarget target, BufferHandle buffer)
{
    m_target = target;
    m_id = buffer;
    m_isGLValid = true;
}

Texture::~Texture()
{
    delete[] d;
    if(m_isGLValid) glDeleteTextures(1, &m_id);
    
}

} // namespace Engine::Rendering


