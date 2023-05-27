#include "SceneManager.hpp"
#include "memory/Allocator.hpp"
#include "memory/MemoryManager.hpp"

#include "core/EngineSettings.hpp"
#include "memory/MemoryManager.hpp"

using namespace Memory;
using namespace Core;

namespace Gameplay
{

SceneManager::SceneManager()
: m_activeScene(nullptr)
{
    
}

void SceneManager::Initialise() 
{
    u64 memory = EngineSettings::Get().Memory.SceneMemory;
    SetAllocator(MemoryManager::GetInstance()->CreateAllocator<SceneManager>(memory));
}

void SceneManager::LoadScene(std::string name)
{
    throw "Not implemented";
}

void SceneManager::UnloadScene(std::string name)
{
    throw "Not implemented";
}

Scene* SceneManager::GetActiveScene()
{
    return m_activeScene;
}

Scene* SceneManager::CreateNewScene(std::string name)
{
    Scene* scene = new (m_allocator->Allocate<Scene>()) Scene(name);
    scene->SetAllocator(m_allocator);
    m_scenes.push_back(scene);
    if(m_activeScene == nullptr) m_activeScene = scene;
    return scene;
}

} // namespace Core
