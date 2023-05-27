#include "Texture.hpp"

#include "Renderer.hpp"
#include "rendering/Debugging.hpp"

namespace Rendering
{

Texture::Texture(TextureTarget target, BufferHandle buffer) : Resource(AssetID())
{
    m_target = target;
    m_id = buffer;
}

Texture::Texture(uint32_t textureID, uint32_t width, uint32_t height, uint32_t channels, AssetID resourceID)
    : Resource(resourceID)
{
    m_target = GL_TEXTURE_2D;
    m_id = textureID;
    m_width = width;
    m_height = height;
    m_channels = channels;
}

Texture::~Texture()
{
    
}

} // namespace Rendering


