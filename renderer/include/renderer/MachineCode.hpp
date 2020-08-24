#pragma once

#include <stdint.h>

namespace Rendering
{
typedef uintptr_t Variable;

enum MachineCode{
    BIND_VAO,
    DRAW_ELEMENTS,
    UPDATE_UNIFORMS,
    BIND_FRAMEBUFFER,
    ENABLE_DEPTHMASK,
    DISABLE_DEPTHMASK,
    PUSH_FLOAT,
    PUSH_FLOAT2,
    PUSH_FLOAT3,
    PUSH_FLOAT4,
    PUSH_INT,
    PUSH_INT2,
    PUSH_INT3,
    PUSH_INT4,
    PUSH_MAT4X4,

};    

} // namespace Rendering


