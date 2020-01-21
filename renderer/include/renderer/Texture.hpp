#pragma once

#include <stdint.h>
#include <vector>
namespace Rendering
{

class Texture
{
private:
    uint32_t _width;
    uint32_t _height;
    uint32_t _channels;
    std::vector<unsigned char> _data;
public:
    Texture(uint32_t width, uint32_t height, uint32_t channels, unsigned char* data);
    
    uint32_t GetWidth();

    uint32_t GetHeight();

    uint32_t GetChannels();

    unsigned char* GetData();
};

} // namespace Rendering
