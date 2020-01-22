#pragma once

#include "renderer/Texture.hpp"

#include <stdint.h>

namespace Rendering
{

class Cubemap
{
private:
    uint32_t _textureID;

    // Texture *right, *left, *top, *bot, *back, *front;

    void GenerateTexture();

    void UploadTextureData(Texture* right, Texture* left, Texture* top, Texture* bot, Texture* back, Texture* front);

public:
    Cubemap(Texture* right, Texture* left, Texture* top, Texture* bot, Texture* back, Texture* front);
    ~Cubemap();

    uint32_t GetTextureID();
};

} // namespace Rendering
