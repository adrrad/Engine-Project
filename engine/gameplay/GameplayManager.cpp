#include "gameplay/GameplayManager.hpp"
#include "core/EngineSettings.hpp"
#include "memory/MemoryManager.hpp"
#include "gameplay/ComponentManager.hpp"
#include "gameplay/SceneManager.hpp"
#include "EngineTypedefs.hpp"
#include "components/ECSComponents.hpp"
#include "ecs/systems/ECSRenderingSystem.hpp"
#include "ecs/systems/ECSSceneGraphSystem.hpp"

using namespace Memory;
using namespace Core;

namespace Gameplay
{
void GameplayManager::Update(float deltaTime) 
{
    m_componentManager->Update(deltaTime);
    Scene* scene = m_sceneManager->GetActiveScene();
    scene->GetStaticTree()->Rebuild();
    scene->GetDynamicTree()->Rebuild();
}

void GameplayManager::Initialise() 
{
    u64 memory = EngineSettings::Get().Memory.GameplayMemory;
    SetAllocator(MemoryManager::GetInstance()->CreateAllocator<GameplayManager>(memory));

    m_sceneManager = SceneManager::GetInstance();
    m_componentManager = ComponentManager::GetInstance();
    SetupECS();
}

void GameplayManager::SetupECS() 
{
    m_ecsManager = ECSManager::GetInstance();
    m_ecsManager->RegisterComponent<ECS::TransformComponent>();
    m_ecsManager->RegisterComponent<ECS::MeshComponent>();
    m_ecsManager->RegisterComponent<ECS::MaterialComponent>();
    m_ecsManager->RegisterComponent<ECS::AxisAlignedBoxComponent>();
    auto renderingSystem = m_ecsManager->RegisterSystem<ECSRenderingSystem>();
    m_ecsManager->SetSystemSignature<ECSRenderingSystem>(renderingSystem->GetSystemSignature());
    auto sceneGraphSystem = m_ecsManager->RegisterSystem<ECSSceneGraphSystem>();
    m_ecsManager->SetSystemSignature<ECSRenderingSystem>(sceneGraphSystem->GetSystemSignature());
}

void GameplayManager::Start() 
{
    m_componentManager->Start();
}

GameplayManager::GameplayManager()
: m_componentManager(nullptr)
, m_sceneManager(nullptr)
{

}
    
} // namespace Gameplay
