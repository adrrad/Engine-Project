#pragma once

#include <stdint.h>

namespace Rendering
{
typedef uintptr_t Variable;

enum MachineCode{
    BIND_VAO,
    DRAW_ELEMENTS,
    BIND_FRAMEBUFFER,
    USE_SHADER,
    BIND_TEXTURE,
    ENABLE_DEPTHMASK,
    DISABLE_DEPTHMASK,
};    

} // namespace Rendering


