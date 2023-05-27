#include "rendering/Renderqueue.hpp"

#include "rendering/MachineCode.hpp"
#include "components/MeshComponent.hpp"

namespace Rendering
{

Renderqueue::Renderqueue(ElementCount maxInstructions, ElementCount maxVarsPerInstruction)
    : m_instructions(maxInstructions)
    , m_variables(maxInstructions * maxVarsPerInstruction)
{
    m_count = 0;
    m_vCount = 0;
    m_ci = 0;
    m_cv = 0;
}

Renderqueue::Renderqueue(Array<MachineCode> instructions, Array<Variable> variables, ElementCount numInstructions, ElementCount numVariables)
    : m_instructions(instructions)
    , m_variables(variables)
{
    m_ci = 0;
    m_cv = 0;
    m_count = numInstructions;
    m_vCount = numVariables;

}

Renderqueue::~Renderqueue()
{
    
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
