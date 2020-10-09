#pragma once

#include "rendering/RenderingTypedefs.hpp"
#include "rendering/Texture.hpp"
#include "assets/resources/CubemapAsset.hpp"
#include "Array.hpp"
#include "EngineData.hpp"

#include <glm/glm.hpp>

#include <stdint.h>
#include <vector>


namespace Engine::Rendering
{

class Cubemap
{
friend class Framebuffer;
private:
    uint32_t m_id = 0;
    uint32_t m_width = 0;
    uint32_t m_height = 0;
    uint32_t m_channels = 0;
    uint32_t m_target = 0;
    Texture m_cubemapTexture;

    Texture CreateCubemap(Engine::ImageData* right, Engine::ImageData* left, Engine::ImageData* top, Engine::ImageData* bot, Engine::ImageData* back, Engine::ImageData* front);

public:

    Cubemap(Engine::ImageData* right, Engine::ImageData* left, Engine::ImageData* top, Engine::ImageData* bot, Engine::ImageData* back, Engine::ImageData* front);

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

} // namespace Engine::Rendering
