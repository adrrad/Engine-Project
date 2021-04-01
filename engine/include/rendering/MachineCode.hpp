#pragma once

#include <stdint.h>

namespace Engine::Rendering
{
typedef uint32_t Variable;

enum MachineCode{
    BIND_VAO,
    DRAW_ELEMENTS,
    BIND_FRAMEBUFFER,
    SET_VIEWPORT,
    USE_SHADER,
    BIND_TEXTURE,
    BIND_UNIFORM_BUFFER_RANGE,
    ENABLE_DEPTHMASK,
    DISABLE_DEPTHMASK,
    CLEAR_DEPTH_BUFFER,
    CLEAR_COLOR_BUFFER,
    ENABLE_BLENDING,
    DISABLE_BLENDING,
    SET_UNIFORM_INT,
};    

} // namespace Engine::Rendering


