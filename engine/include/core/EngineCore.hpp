#pragma once
#include "core/EngineSubsystem.hpp"
#include "rendering/Renderer.hpp"

#include <vector>


namespace Engine::Core
{

class EngineCore
{
private:

    std::vector<EngineSubsystem*> subsystems;


    void Initialise();
    
    void UpdateSubsystems();

public:

    EngineCore();

    void GameLoop();

};

} // namespace Engine::Core
