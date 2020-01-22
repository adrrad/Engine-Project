#include "renderer/Texture.hpp"

#include <glad/glad.h>

namespace Rendering
{

void Texture::UploadTexture(uint32_t glTarget)
{
    glGenTextures(1, &_id);
    glBindTexture(GL_TEXTURE_2D, _id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(
        glTarget,
        0,
        GL_RGB,
        _width,
        _height,
        0,
        GL_RGB,
        GL_UNSIGNED_BYTE,
        d);
    glGenerateMipmap(GL_TEXTURE_2D);

}

Texture::Texture(uint32_t width, uint32_t height, uint32_t channels, unsigned char* data)
{
    _width = width;
    _height = height;
    _channels = channels;
    _data = std::vector<unsigned char>(data, data+(width*height));
    d = data;
}

Texture::Texture(uint32_t width, uint32_t height, uint32_t channels, unsigned char* data, uint32_t glTarget) : Texture(width, height, channels, data)
{
    UploadTexture(glTarget);
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

unsigned char* Texture::GetData()
{
    return d;
}


} // namespace Rendering


