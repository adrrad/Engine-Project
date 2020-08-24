#pragma once

#include "renderer/MachineCode.hpp"
#include "renderer/Renderqueue.hpp"


namespace Rendering
{
class Renderer;    
class Machine
{

private:
    Renderer* _renderer = nullptr;
public:

    Machine();

    void Run(Renderqueue* queue);
};

} // namespace Rendering
