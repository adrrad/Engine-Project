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

    void CreateCubemap(Texture* right, Texture* left, Texture* top, Texture* bot, Texture* back, Texture* front);
public:
    // As data storage
    Texture(uint32_t width, uint32_t height, uint32_t channels, unsigned char* data);
    // As texture (GL_TEXTURE_2D)
    Texture(uint32_t width, uint32_t height, uint32_t channels, unsigned char* data, uint32_t glTarget);
    // As Cubemap
    Texture(Texture* right, Texture* left, Texture* top, Texture* bot, Texture* back, Texture* front);

    void BindTexture();
    
    uint32_t GetID();

    uint32_t GetWidth();

    uint32_t GetHeight();

    uint32_t GetChannels();

    uint32_t GetType();

    unsigned char* GetData();
};

} // namespace Rendering
