#include "renderer/Renderqueue.hpp"

#include "renderer/MachineCode.hpp"
#include "components/MeshComponent.hpp"

namespace Rendering
{

Renderqueue::Renderqueue(ElementCount maxInstructions, ElementCount maxVarsPerInstruction)
{
    _count = 0;
    _vCount = 0;
    _ci = 0;
    _cv = 0;

    _maxInstructions = maxInstructions;
    _maxVariables = maxInstructions * maxVarsPerInstruction;
    _instructions = new MachineCode[maxInstructions];
    _vars = new Variable[_maxVariables];
}

Renderqueue::Renderqueue(MachineCode* instructions, Variable* variables, ElementCount numInstructions, ElementCount numVariables)
{
    _ci = 0;
    _cv = 0;
    _count = numInstructions;
    _vCount = numVariables;
    _maxInstructions = numInstructions;
    _maxVariables = numVariables;
    _instructions = instructions;
    _vars = variables;
}

Renderqueue::~Renderqueue()
{
    delete[] _instructions;
    delete[] _vars;
}

void Renderqueue::Push(uint32_t vao, uint32_t topology, uint32_t elementCount)
{
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
