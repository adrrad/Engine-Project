#include "rendering/Cubemap.hpp"


#include <glad/glad.h>

namespace Rendering
{

Texture Cubemap::CreateCubemap(ImageData* right, ImageData* left, ImageData* top, ImageData* bot, ImageData* back, ImageData* front)
{
    ImageData* textures[] = {right, left, top, bot, front, back};
    glGenTextures(1, &m_id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_id);
    for(u64 texIndex = 0; texIndex < 6; texIndex++)
    {
        ImageData* tex = textures[texIndex];
        glTexImage2D(
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + texIndex, 
            0, 
            GL_RGB, 
            tex->Width, 
            tex->Height, 
            0, 
            GL_RGB, 
            GL_UNSIGNED_BYTE, 
            tex->Pixels.Data());
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    return Texture(GL_TEXTURE_CUBE_MAP, m_id);
}

Cubemap::Cubemap(ImageData* right, ImageData* left, 
                 ImageData* top, ImageData* bot, 
                 ImageData* back, ImageData* front,
                 AssetID resourceID)
    : Resource(resourceID), m_cubemapTexture(CreateCubemap(right, left, top, bot, back, front)) 
{

}

Cubemap::~Cubemap()
{
    
}

} // namespace Rendering
