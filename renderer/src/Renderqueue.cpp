#include "renderer/Renderqueue.hpp"

#include "renderer/MachineCode.hpp"
#include "components/MeshComponent.hpp"

namespace Rendering
{

void Renderqueue::PushInstruction(MachineCode i)
{
    _instructions[_count] = i;
    _count++;
}

void Renderqueue::PushVariable(Variable v)
{
    _vars[_vCount] = v;
    _vCount++;
}

Renderqueue::Renderqueue(uint32_t capacity)
{
    _count = 0;
    _vCount = 0;
    _ci = 0;
    _cv = 0;

    _capacity = capacity;
    _instructions = new MachineCode[capacity];
    _vars = new Variable[capacity];
}


void Renderqueue::Push(uint32_t vao, uint32_t topology, uint32_t elementCount, Components::MeshComponent* mat)
{
    if(_count == _capacity) throw std::exception("Renderqueue error: trying to push to a full queue!");
    PushInstruction(MachineCode::UPDATE_UNIFORMS);
    PushVariable((Variable)mat);
    PushInstruction(MachineCode::BIND_VAO);
    PushVariable(vao);
    PushInstruction(MachineCode::DRAW_ELEMENTS);
    PushVariable(topology);
    PushVariable(elementCount);
}

void Renderqueue::UseFramebuffer(BufferHandle fbo)
{
    PushInstruction(MachineCode::BIND_FRAMEBUFFER);
    PushVariable(fbo);
}

void Renderqueue::UseDepthMask(bool flag)
{
    PushInstruction(flag ? MachineCode::ENABLE_DEPTHMASK : MachineCode::DISABLE_DEPTHMASK);
}

} // namespace Rendering
