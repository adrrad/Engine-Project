#pragma once

#include "EngineComponent.hpp"
#include "MachineCode.hpp"
#include "Renderqueue.hpp"


namespace Rendering
{
class Renderer;    
class RVM : public SubsystemComponent<Renderer>
{

private:
    Renderer* m_renderer = nullptr;
public:

    RVM();

    void Run(Renderqueue* queue);
};

} // namespace Rendering
