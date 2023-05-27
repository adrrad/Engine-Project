#pragma once

#include "rendering/RenderingTypedefs.hpp"
#include "rendering/Texture.hpp"
#include "assets/resources/CubemapAsset.hpp"
#include "Array.hpp"
#include "EngineData.hpp"
#include "Resource.hpp"

#include <glm/glm.hpp>

#include <stdint.h>
#include <vector>


namespace Rendering
{

class Cubemap : public Resource
{
friend class Framebuffer;
private:
    uint32_t m_id = 0;
    uint32_t m_width = 0;
    uint32_t m_height = 0;
    uint32_t m_channels = 0;
    uint32_t m_target = 0;
    Texture m_cubemapTexture;

    Texture CreateCubemap(ImageData* right, ImageData* left, ImageData* top, ImageData* bot, ImageData* back, ImageData* front);

public:

    Cubemap(ImageData* right, ImageData* left, 
        ImageData* top, ImageData* bot, 
        ImageData* back, ImageData* front,
        AssetID resourceID);

    ~Cubemap();

    __forceinline uint32_t GetID();

    __forceinline uint32_t GetWidth();

    __forceinline uint32_t GetHeight();

    __forceinline Texture* GetTexture();
};

uint32_t Cubemap::GetID()
{
    return m_id;
}

uint32_t Cubemap::GetWidth()
{
    return m_width;
}

uint32_t Cubemap::GetHeight()
{
    return m_height;
}

Texture* Cubemap::GetTexture() 
{ 
    return &m_cubemapTexture;
}

} // namespace Rendering
