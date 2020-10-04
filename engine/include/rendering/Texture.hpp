#pragma once

#include "rendering/RenderingTypedefs.hpp"
#include "Array.hpp"
#include "EngineData.hpp"

#include "assets/resources/ImageAsset.hpp"
#include "assets/resources/CubemapAsset.hpp"

#include <glm/glm.hpp>

#include <stdint.h>
#include <vector>
namespace Engine::Rendering
{

template<Size W, Size H>
struct SamplingKernel
{
    const Size Width = W, Height = H;
    float Weights[W][H];
    SamplingKernel(float weights[W][H]) 
    { 
        if(Width % 2 != 1 || Height % 2 != 1) throw std::exception("Kernel must have uneven dimensions!");
        memcpy(Weights, weights, sizeof(float)*W*H);
    }
};

class Texture
{
friend class Framebuffer;
private:
    uint32_t m_id = 0;
    uint32_t m_width = 0;
    uint32_t m_height = 0;
    uint32_t m_channels = 0;
    uint32_t m_target = 0;
    bool m_isGLValid;
    unsigned char* d;

    void UploadTexture();

    void CreateCubemap(Texture* right, Texture* left, Texture* top, Texture* bot, Texture* back, Texture* front);

    /**
     * @brief Sets the value indicating the texture's validity in the context of the graphics API. Do not set to false if the object contains a valid texture handle.
     * 
     */
    inline void SetValid(bool isValid) { m_isGLValid = isValid; }
public:
    // As data storage
    Texture(ImageData* data);
    Texture(uint32_t width, uint32_t height, uint32_t channels, unsigned char* data);
    // As texture (GL_TEXTURE_2D)
    Texture(uint32_t width, uint32_t height, uint32_t channels, unsigned char* data, uint32_t glTarget);
    // As Cubemap
    Texture(Texture* right, Texture* left, Texture* top, Texture* bot, Texture* back, Texture* front);
    Texture(Assets::ImageAsset* right, Assets::ImageAsset* left, Assets::ImageAsset* top, 
            Assets::ImageAsset* bot, Assets::ImageAsset* back, Assets::ImageAsset* front);
    Texture(Assets::CubemapAsset* cubemap);
    //As buffer
    Texture(TextureTarget target, BufferHandle buffer);

    ~Texture();

    /**
     * @brief Returns a boolean value indicating whether the texture is valid in the context of the graphics API.
     * 
     */
    inline bool IsValid() { return m_isGLValid; }
    
    inline uint32_t GetID();

    inline uint32_t GetWidth();

    inline uint32_t GetHeight();

    inline uint32_t GetChannels();

    inline uint32_t GetType();

    inline unsigned char* GetData();

    inline glm::vec4 Sample(int x, int y);

    template<int Width, int Height>
    glm::vec4 KernelSample(int x, int y, SamplingKernel<Width, Height> kernel);
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

unsigned char* Texture::GetData()
{
    return d;
}

inline glm::vec4 Texture::Sample(int x, int y)
{
    int offset = m_channels*y*m_width + m_channels*x;
    float r = 0.0f, g = 0.0f, b = 0.0f, a = 0.0f;
    r = m_channels >= 1 ? d[offset] : 0.0f;
    g = m_channels >= 2 ? d[offset+1] : 0.0f;
    b = m_channels >= 3 ? d[offset+2] : 0.0f;
    a = m_channels >= 4 ? d[offset+3] : 0.0f;
    return glm::vec4(r, g, b, a);
}

template<int Width, int Height>
inline glm::vec4 Texture::KernelSample(int x, int y, SamplingKernel<Width, Height> kernel)
{
    glm::vec4 sum = glm::vec4(0.0f);
    int halfW = kernel.Width / 2;
    int halfH = kernel.Height / 2;
    for(Size ky = 0; ky < kernel.Height; ky++)
    {
        //Calculate texel coordinates and clamp within the possible texture size.
        int ty = y - halfH + ky;
        ty = ty < 0.0f ? 0.0f : ty;
        ty = ty > m_height-1 ? m_height-1 : ty;

        for(Size kx = 0; kx < kernel.Width; kx++)    
        {
            int tx = x - halfW + kx;
            tx = tx < 0.0f ? 0.0f : tx;
            tx = tx > m_width-1 ? m_width-1 : tx;
            sum += Sample(tx, ty) * kernel.Weights[kx][ky];
        }   
    }
    return sum * (1.0f/(kernel.Width*kernel.Height));
}

} // namespace Engine::Rendering
