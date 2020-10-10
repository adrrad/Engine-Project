#pragma once

#include "EngineTypedefs.hpp"
#include "rendering/MachineCode.hpp"
#include "rendering/RenderingTypedefs.hpp"

#include <stdint.h>

namespace Engine::Components
{
    class MeshComponent;
}

namespace Engine::Rendering
{
class Renderqueue
{
friend class Renderpass;
private:
    uint32_t _maxInstructions;
    uint32_t _maxVariables;
    uint32_t _count;
    uint32_t _vCount;
    uint32_t _ci, _cv; // Current instruction, current variable

    MachineCode* _instructions;
    Variable* _vars;

    __forceinline void PushInstruction(MachineCode i)
    {
        if(_count == _maxInstructions) throw std::exception("Renderqueue instruction capacity reached!");
        _instructions[_count] = i;
        _count++;
    }

    __forceinline void PushVariable(Variable v)
    {
        if(_vCount == _maxVariables) throw std::exception("Renderqueue variable capacity reached!");
        _vars[_vCount] = v;
        _vCount++;
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
    Renderqueue(MachineCode* instructions, Variable* variables, ElementCount numInstructions, ElementCount numVariables);

    ~Renderqueue();

    void Push(uint32_t vao, uint32_t topology, uint32_t elementCount);

    void UseFramebuffer(BufferHandle fbo);

    void UseDepthMask(bool flag);

    __forceinline MachineCode* GetInstructions()
    {
        return _instructions;
    }

    __forceinline uint32_t GetInstructionsCount()
    {
        return _count;
    }

    __forceinline Variable* GetVariables()
    {
        return _vars;
    }

    __forceinline uint32_t GetVariablesCount()
    {
        return _vCount;
    }

    __forceinline MachineCode NextInstruction()
    {
        auto& i = _instructions[_ci];
        _ci++;
        return i;
    }

    __forceinline Variable NextVariable()
    {
        auto& v = _vars[_cv];
        _cv++;
        return v;
    }

    __forceinline bool HasNextInstruction()
    {
        return _ci < _count;
    }

    __forceinline void ResetIndexCounters()
    {
        _ci = 0;
        _cv = 0;
    }
};

} // namespace Engine::Rendering
