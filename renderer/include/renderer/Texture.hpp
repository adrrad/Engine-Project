#pragma once

#include <stdint.h>
#include <vector>
namespace Rendering
{

class Texture
{
private:
    // ID is used in case of the second constructor
    uint32_t _id = 0;
    uint32_t _width = 0;
    uint32_t _height = 0;
    uint32_t _channels = 0;
    uint32_t _target = 0;
    std::vector<unsigned char> _data;
    unsigned char* d;
    void UploadTexture();

public:
    Texture(uint32_t width, uint32_t height, uint32_t channels, unsigned char* data);

    Texture(uint32_t width, uint32_t height, uint32_t channels, unsigned char* data, uint32_t glTarget);

    void BindTexture();
    
    uint32_t GetID();

    uint32_t GetWidth();

    uint32_t GetHeight();

    uint32_t GetChannels();

    unsigned char* GetData();
};

} // namespace Rendering
