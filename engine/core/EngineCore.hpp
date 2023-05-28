#pragma once
#include "EngineTypedefs.hpp"
#include "gameplay/SceneManager.hpp"
#include "core/EngineSettings.hpp"
#include "core/EngineSubsystem.hpp"
#include "rendering/Renderer.hpp"
#include "rendering/HLRenderer.hpp"
#include "physics/PhysicsManager.hpp"
#include "gameplay/ComponentManager.hpp"
#include "memory/MemoryManager.hpp"
#include "assets/AssetManager.hpp"
#include "gameplay/GameplayManager.hpp"

#include <vector>

namespace Editor
{
    class EditorCore;
}

namespace Core
{


class EngineCore
{
friend class Editor::EditorCore;
private:
    u32 mainWindow;
    Platform::WindowManager* windowManager;
    Physics::PhysicsManager* physicsManager;
    Assets::AssetManager* assetManager;
    Gameplay::GameplayManager* gameplayManager;
    Rendering::HLRenderer* hlrenderer;
    Memory::MemoryManager* memoryManager;
    Rendering::Renderer* renderer;
    Gameplay::SceneManager* sceneManager;

    void InitialiseEngine();

    void InitialiseSettings();

    void InitialiseUtilities();

    void InitialiseSubsystems();
    
    void UpdateSubsystems(float deltaTime);

    bool ShouldClose();

    void Close();

    template<typename T>
    T* InitialiseSubsystem();

    /**
     * @brief Run a single iteration of the game loop. Used by the EditorCore.
     * 
     */
    void LoopIteration();
public:

    EngineCore();

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

template<typename T>
T* EngineCore::InitialiseSubsystem()
{
    T* subsystem = T::GetInstance();
    subsystem->Initialise();
    return subsystem;
}

} // namespace Core
