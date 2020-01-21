#include "renderer/Texture.hpp"

namespace Rendering
{

Texture::Texture(uint32_t width, uint32_t height, uint32_t channels, unsigned char* data)
{
    _width = width;
    _height = height;
    _channels = channels;
    _data.resize(width*height); 
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
    return _data.data();
}


} // namespace Rendering


