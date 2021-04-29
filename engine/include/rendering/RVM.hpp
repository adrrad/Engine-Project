#pragma once

#include "rendering/MachineCode.hpp"
#include "rendering/Renderqueue.hpp"


namespace Engine::Rendering
{
class Renderer;    
class RVM
{

private:
    Renderer* m_renderer = nullptr;
public:

    RVM();

    void Run(Renderqueue* queue);
};

} // namespace Engine::Rendering
