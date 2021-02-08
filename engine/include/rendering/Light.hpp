#pragma once

#include "rendering/RenderingStructs.hpp"

namespace Engine::Rendering
{
    
struct LightBuffer
{
    uint32_t InstanceHandle;
    uint32_t BindingIndex;
    uint32_t Buffer;
    uint32_t Offset;
    uint32_t Size;

    LightBuffer()
    {
        InstanceHandle = 0;
        BindingIndex = 0;
        Buffer = 0;
        Offset = 0;
        Size = 0;
    }
};


} // namespace Engine::Rendering
