#pragma once

#include "rendering/RenderingTypedefs.hpp"

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
        for(int i = 0; i < W; i++)
            for(int j = 0; j < H; j++)
                Weights[i][j] = weights[i][j];
    }
};

class Texture
{
friend class Framebuffer;
private:
    uint32_t _id = 0;
    uint32_t _width = 0;
    uint32_t _height = 0;
    uint32_t _channels = 0;
    uint32_t _target = 0;
    bool _isGLValid;
    std::vector<unsigned char> _data;
    unsigned char* d;

    void UploadTexture();

    void CreateCubemap(Texture* right, Texture* left, Texture* top, Texture* bot, Texture* back, Texture* front);

    /**
     * @brief Sets the value indicating the texture's validity in the context of the graphics API. Do not set to false if the object contains a valid texture handle.
     * 
     */
    inline void SetValid(bool isValid) { _isGLValid = isValid; }
public:
    // As data storage
    Texture(uint32_t width, uint32_t height, uint32_t channels, unsigned char* data);
    // As texture (GL_TEXTURE_2D)
    Texture(uint32_t width, uint32_t height, uint32_t channels, unsigned char* data, uint32_t glTarget);
    // As Cubemap
    Texture(Texture* right, Texture* left, Texture* top, Texture* bot, Texture* back, Texture* front);
    //As buffer
    Texture(TextureTarget target, BufferHandle buffer);

    ~Texture();

    void BindTexture();

    /**
     * @brief Returns a boolean value indicating whether the texture is valid in the context of the graphics API.
     * 
     */
    inline bool IsValid() { return _isGLValid; }
    
    uint32_t GetID();

    uint32_t GetWidth();

    uint32_t GetHeight();

    uint32_t GetChannels();

    uint32_t GetType();

    unsigned char* GetData();

    inline glm::vec4 Sample(int x, int y);

    template<int Width, int Height>
    glm::vec4 KernelSample(int x, int y, SamplingKernel<Width, Height> kernel);
};

inline glm::vec4 Texture::Sample(int x, int y)
{
    int offset = _channels*y*_width + _channels*x;
    float r = 0.0f, g = 0.0f, b = 0.0f, a = 0.0f;
    r = _channels >= 1 ? _data[offset] : 0.0f;
    g = _channels >= 2 ? _data[offset+1] : 0.0f;
    b = _channels >= 3 ? _data[offset+2] : 0.0f;
    a = _channels >= 4 ? _data[offset+3] : 0.0f;
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
        ty = ty > _height-1 ? _height-1 : ty;

        for(Size kx = 0; kx < kernel.Width; kx++)    
        {
            int tx = x - halfW + kx;
            tx = tx < 0.0f ? 0.0f : tx;
            tx = tx > _width-1 ? _width-1 : tx;
            sum += Sample(tx, ty) * kernel.Weights[kx][ky];
        }   
    }
    return sum * (1.0f/(kernel.Width*kernel.Height));
}

} // namespace Engine::Rendering
