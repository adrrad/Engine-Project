#pragma once

#include "rendering/MachineCode.hpp"
#include "rendering/Renderqueue.hpp"


namespace Engine::Rendering
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

} // namespace Engine::Rendering