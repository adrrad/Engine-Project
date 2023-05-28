#pragma once

#include "EngineTypedefs.hpp"
#include "EngineData.hpp"


namespace Utilities
{

template<u32 W, u32 H>
struct SamplingKernel
{
    const u32 Width = W, Height = H;
    float Weights[W][H];
    SamplingKernel(float weights[W][H]) 
    { 
        if(Width % 2 != 1 || Height % 2 != 1) throw std::exception("Kernel must have uneven dimensions!");
        memcpy(Weights, weights, sizeof(float)*W*H);
    }
};

inline glm::vec4 Sample(int x, int y, ImageData* img)
{
    int channels = img->NumChannels;
    int offset = channels*y*img->Width + channels*x;
    auto d = img->Pixels.Data();
    float r = 0.0f, g = 0.0f, b = 0.0f, a = 0.0f;
    r = channels >= 1 ? d[offset] : 0.0f;
    g = channels >= 2 ? d[offset+1] : 0.0f;
    b = channels >= 3 ? d[offset+2] : 0.0f;
    a = channels >= 4 ? d[offset+3] : 0.0f;
    return glm::vec4(r, g, b, a);
}

template<int Width, int Height>
glm::vec4 KernelSample(int x, int y, SamplingKernel<Width, Height> kernel, ImageData* img)
{
    int height = img->Height, width = img->Width;
    glm::vec4 sum = glm::vec4(0.0f);
    int halfW = kernel.Width / 2;
    int halfH = kernel.Height / 2;
    for(u32 ky = 0; ky < kernel.Height; ky++)
    {
        //Calculate texel coordinates and clamp within the possible texture size.
        int ty = y - halfH + ky;
        ty = ty < 0.0f ? 0.0f : ty;
        ty = ty > height-1 ? height-1 : ty;

        for(u32 kx = 0; kx < kernel.Width; kx++)    
        {
            int tx = x - halfW + kx;
            tx = tx < 0.0f ? 0.0f : tx;
            tx = tx > width-1 ? width-1 : tx;
            sum += Sample(tx, ty, img) * kernel.Weights[kx][ky];
        }   
    }
    return sum * (1.0f/(kernel.Width*kernel.Height));
}

} // namespace Utilities
