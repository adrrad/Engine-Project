#pragma once

#include "renderer/MachineCode.hpp"
#include "renderer/RenderingTypedefs.hpp"

#include <stdint.h>

namespace Components
{
    class MeshComponent;
}

namespace Rendering
{
typedef float UniformF;
typedef int UniformI;

class Renderqueue
{
friend class Renderpass;
private:
    uint32_t _capacity;
    uint32_t _count;
    uint32_t _vCount;
    uint32_t _ci, _cv; // Current instruction, current variable

    MachineCode* _instructions;
    Variable* _vars;
    UniformF* _ufdata;
    UniformI* _uidata;

    inline void PushInstruction(MachineCode i);

    inline void PushVariable(Variable v);

public:

    Renderqueue(uint32_t capacity);

    void Push(uint32_t vao, uint32_t topology, uint32_t elementCount, Components::MeshComponent* mat);

    void UseFramebuffer(BufferHandle fbo);

    void UseDepthMask(bool flag);

    inline MachineCode* GetInstructions()
    {
        return _instructions;
    }

    inline uint32_t GetInstructionsCount()
    {
        return _count;
    }

    inline Variable* GetVariables()
    {
        return _vars;
    }

    inline uint32_t GetVariablesCount()
    {
        return _vCount;
    }

    inline MachineCode NextInstruction()
    {
        auto& i = _instructions[_ci];
        _ci++;
        return i;
    }

    inline Variable NextVariable()
    {
        auto& v = _vars[_cv];
        _cv++;
        return v;
    }

    inline bool HasNextInstruction()
    {
        return _ci < _count;
    }

    inline void ResetIndexCounters()
    {
        _ci = 0;
        _cv = 0;
    }
};

} // namespace Rendering
