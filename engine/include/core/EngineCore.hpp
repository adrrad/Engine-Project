#pragma once
#include "EngineTypedefs.hpp"
#include "core/EngineSettings.hpp"
#include "core/EngineSubsystem.hpp"
#include "rendering/Renderer.hpp"
#include "physics/PhysicsManager.hpp"
#include "components/ComponentManager.hpp"


#include <vector>

namespace Engine::Editor
{
    class EditorCore;
}

namespace Engine::Core
{


class EngineCore
{
friend class Engine::Editor::EditorCore;
private:
    WindowHandle mainWindow;
    EngineSettings settings;
    Platform::WindowManager* windowManager;
    Rendering::Renderer* renderer;
    Physics::PhysicsManager* physicsManager;
    Components::ComponentManager* componentManager;

    std::vector<EngineSubsystem*> subsystems;

    void InitialiseEngine();

    void InitialiseUtilities();

    void InitialiseSubsystems();
    
    void UpdateSubsystems(float deltaTime);

    bool ShouldClose();

    void Close();

    /**
     * @brief Run a single iteration of the game loop. Used by the EditorCore.
     * 
     */
    void LoopIteration();
public:

    EngineCore(EngineSettings settings);

    /**
     * @brief Start the game before the game loop, calling each game object's components' start functions.
     * 
     */
    void Start();

    /**
     * @brief Starts the game loop, running until the game is closed.
     * 
     */
    void GameLoop();

};

} // namespace Engine::Core
