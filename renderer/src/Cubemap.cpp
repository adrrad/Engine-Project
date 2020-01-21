#include "renderer/Cubemap.hpp"
#include <glad/glad.h>

namespace Rendering
{

void Cubemap::GenerateTexture()
{
    glGenTextures(1, &textureID);
}

void Cubemap::UploadTextureData(Texture* right, Texture* left, Texture* top, Texture* bot, Texture* back, Texture* front)
{
    Texture* textures[] = {right, left, top, bot, back, front};
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
    
    for(uint32_t texIndex = 0; texIndex < 6; texIndex++)
    {
        Texture* tex = textures[texIndex];
        glTexImage2D(
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + texIndex, 
            0, 
            GL_RGB, 
            tex->GetWidth(), 
            tex->GetHeight(), 
            0, 
            GL_RGB, 
            GL_UNSIGNED_BYTE, 
            tex->GetData());
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}


Cubemap::Cubemap(Texture* right, Texture* left, Texture* top, Texture* bot, Texture* back, Texture* front)
{
    GenerateTexture();
    UploadTextureData(0,0,0,0,0,0);
}

Cubemap::~Cubemap()
{

}
    
} // namespace Rendering


