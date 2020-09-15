#pragma once
#include "core/EngineSubsystem.hpp"
#include "rendering/Renderer.hpp"
#include "physics/PhysicsManager.hpp"
#include "components/ComponentManager.hpp"
#include <vector>


namespace Engine::Core
{

class EngineCore
{
private:

    Rendering::Renderer* renderer;
    Physics::PhysicsManager* physicsManager;
    Components::ComponentManager* componentManager;

    std::vector<EngineSubsystem*> subsystems;

    void Initialise();
    
    void UpdateSubsystems(float deltaTime);

    bool ShouldClose();

    void Close();

public:

    EngineCore();

    void GameLoop();

};

} // namespace Engine::Core
