#pragma once

#include "EngineTypedefs.hpp"
#include "rendering/MachineCode.hpp"
#include "rendering/RenderingTypedefs.hpp"

#include "Array.hpp"

#include <stdint.h>

namespace Gameplay
{
    class MeshComponent;
}

namespace Rendering
{
class Renderqueue
{
friend class Renderpass;
private:
    uint32_t m_count;
    uint32_t m_vCount;
    uint32_t m_ci, m_cv; // Current instruction, current variable

    Array<MachineCode> m_instructions;
    Array<Variable> m_variables;

    __forceinline void PushInstruction(MachineCode i)
    {
        if(m_count == m_instructions.Length) throw std::exception("Renderqueue instruction capacity reached!");
        m_instructions[m_count] = i;
        m_count++;
    }

    __forceinline void PushVariable(Variable v)
    {
        if(m_vCount == m_variables.Length) throw std::exception("Renderqueue variable capacity reached!");
        m_variables[m_vCount] = v;
        m_vCount++;
    }
public:

    /**
     * @brief Construct a new Renderqueue for recording new commands
     * 
     * @param maxInstructions The maximum number of instructions to be recorded in the renderqueue
     * @param maxVarsPerInstructions The maximum number of possible instructions per instruction. Used to determine the variable array allocation.
     */
    Renderqueue(ElementCount maxInstructions, ElementCount maxVarsPerInstruction = 5);

    /**
     * @brief Construct a new Renderqueue with specified instructions and variables.
     * 
     * @param instructions 
     * @param variables 
     * @param numInstructions 
     * @param numVariables 
     */
    Renderqueue(Array<MachineCode> instructions, Array<Variable> variables, ElementCount numInstructions, ElementCount numVariables);

    ~Renderqueue();

    void Push(uint32_t vao, uint32_t topology, uint32_t elementCount);

    void UseFramebuffer(BufferHandle fbo);

    void UseDepthMask(bool flag);

    inline Array<MachineCode>& GetInstructions()
    {
        return m_instructions;
    }

    inline uint32_t GetInstructionsCount()
    {
        return m_count;
    }

    inline Array<Variable>& GetVariables()
    {
        return m_variables;
    }

    inline uint32_t GetVariablesCount()
    {
        return m_vCount;
    }

    inline MachineCode NextInstruction()
    {
        auto& i = m_instructions[m_ci];
        m_ci++;
        return i;
    }

    inline Variable NextVariable()
    {
        auto& v = m_variables[m_cv];
        m_cv++;
        return v;
    }

    inline bool HasNextInstruction()
    {
        return m_ci < m_count;
    }

    inline void ResetIndexCounters()
    {
        m_ci = 0;
        m_cv = 0;
    }
};

} // namespace Rendering
