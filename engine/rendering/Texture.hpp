#pragma once

#include "EngineComponent.hpp"
#include "RenderingTypedefs.hpp"
#include "Array.hpp"
#include "EngineData.hpp"
#include "Resource.hpp"
#include "EngineTypedefs.hpp"

#include "assets/resources/ImageAsset.hpp"
#include "assets/resources/CubemapAsset.hpp"

#include <glm/glm.hpp>

#include <stdint.h>
#include <vector>
namespace Rendering
{
class Renderer;
class Texture  : public SubsystemComponent<Renderer>, public Resource
{
friend class Framebuffer;
private:
    uint32_t m_id = 0;
    uint32_t m_width = 0;
    uint32_t m_height = 0;
    uint32_t m_channels = 0;
    uint32_t m_target = 0;

public:

    Texture(TextureTarget target, BufferHandle buffer);

    Texture(uint32_t textureID, uint32_t width, uint32_t height, uint32_t channels, AssetID resourceID);

    ~Texture();

    inline uint32_t GetID();

    inline uint32_t GetWidth();

    inline uint32_t GetHeight();

    inline uint32_t GetChannels();

    inline uint32_t GetType();

};


uint32_t Texture::GetID()
{
    return m_id;
}

uint32_t Texture::GetWidth()
{
    return m_width;
}

uint32_t Texture::GetHeight()
{
    return m_height;
}

uint32_t Texture::GetChannels()
{
    return m_channels;
}

uint32_t Texture::GetType()
{
    return m_target;
}


} // namespace Rendering
