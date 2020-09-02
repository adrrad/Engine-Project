#include "renderer/Texture.hpp"

#include "renderer/Debugging.hpp"

#include <glad/glad.h>

namespace Rendering
{

void Texture::UploadTexture()
{
    UPDATE_CALLINFO();
    glGenTextures(1, &_id);
    glBindTexture(GL_TEXTURE_2D, _id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    int format = _channels == 3 ? GL_RGB : GL_DEPTH_COMPONENT;
    if(_channels == 4) format = GL_RGBA;
    UPDATE_CALLINFO();
    glTexImage2D(
        _target,
        0,
        format,
        _width,
        _height,
        0,
        format,
        GL_UNSIGNED_BYTE,
        d);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    _isGLValid = true;
    
}

void Texture::CreateCubemap(Texture* right, Texture* left, Texture* top, Texture* bot, Texture* back, Texture* front)
{
    Texture* textures[] = {right, left, top, bot, front, back};
    glGenTextures(1, &_id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, _id);
    
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

Texture::Texture(uint32_t width, uint32_t height, uint32_t channels, unsigned char* data)
{
    _width = width;
    _height = height;
    _channels = channels;
    _data = std::vector<unsigned char>(data, data+(width*height*_channels));
    d = data;
}

Texture::Texture(uint32_t width, uint32_t height, uint32_t channels, unsigned char* data, uint32_t glTarget) : Texture(width, height, channels, data)
{
    _target = glTarget;
    UploadTexture();
}

Texture::Texture(Texture* right, Texture* left, Texture* top, Texture* bot, Texture* back, Texture* front)
{
    _target = GL_TEXTURE_CUBE_MAP;
    CreateCubemap(right, left, top, bot, back, front);
}

Texture::Texture(TextureTarget target, BufferHandle buffer)
{
    _target = target;
    _id = buffer;
    _isGLValid = true;
}

Texture::~Texture()
{
    delete[] d;
    if(_isGLValid) glDeleteTextures(1, &_id);
    
}

void Texture::BindTexture()
{
    glBindTexture(_target, _id);
}

uint32_t Texture::GetID()
{
    return _id;
}

uint32_t Texture::GetWidth()
{
    return _width;
}

uint32_t Texture::GetHeight()
{
    return _height;
}

uint32_t Texture::GetChannels()
{
    return _channels;
}

uint32_t Texture::GetType()
{
    return _target;
}

unsigned char* Texture::GetData()
{
    return d;
}


} // namespace Rendering


