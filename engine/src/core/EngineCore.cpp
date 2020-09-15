#include "core/EngineCore.hpp"


namespace Engine::Core
{

void EngineCore::Initialise()
{
    renderer = Rendering::Renderer::GetInstance();
    physicsManager = Physics::PhysicsManager::GetInstance();
    componentManager = Components::ComponentManager::GetInstance();
    subsystems = {renderer, physicsManager, componentManager};
}

void EngineCore::UpdateSubsystems(float deltaTime)
{
    for(auto system : subsystems) system->Update(deltaTime);
}

bool EngineCore::ShouldClose()
{
    return false;
}

void EngineCore::Close()
{

}

EngineCore::EngineCore()
{
    Initialise();
}

void EngineCore::GameLoop()
{

}

} // namespace Engine::Core