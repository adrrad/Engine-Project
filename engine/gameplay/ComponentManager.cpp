#include "gameplay/ComponentManager.hpp"
#include "components/StandardInclude.hpp"
#include "gameplay/GameObject.hpp"
#include "core/EngineSettings.hpp"
#include "memory/MemoryManager.hpp"
#include "Debug.hpp"

using namespace Core;
using namespace Memory;

namespace Gameplay
{

std::vector<IComponentPool*> ComponentManager::m_pools;
std::unordered_map<std::string, IComponentPool*> ComponentManager::m_mapping;

BaseComponent* ComponentManager::DeserialiseComponent(Gameplay::GameObject* go, std::shared_ptr<Utilities::JSON::JSONValue> json)
{
    std::string componentName = json->GetMember("name")->String;
    if(m_mapping.contains(componentName))
    {
        BaseComponent* comp = m_mapping[componentName]->AllocateNewComponent();
        comp->gameObject = go;
        comp->Deserialise(json);
        go->m_components.insert({componentName, comp});
        return comp;
    }
    AssertFail("Could not deserialise component '" + componentName + "'");
}

void ComponentManager::Start()
{
    for(auto pool : m_pools)
    {
        pool->Start();
    }
}

void ComponentManager::Update(float deltaTime)
{
    for(auto pool : m_pools)
    {
        pool->Update(deltaTime);
    }
}

void ComponentManager::Initialise() 
{
    u64 memory = EngineSettings::Get().Memory.ComponentMemory;
    SetAllocator(MemoryManager::GetInstance()->CreateAllocator<ComponentManager>(memory));

    RegisterComponentPool<Gameplay::CameraComponent>();
    RegisterComponentPool<Gameplay::InspectorCameraComponent>();
    RegisterComponentPool<Gameplay::MeshComponent>();
    RegisterComponentPool<Gameplay::RigidBodyComponent>();
    RegisterComponentPool<Gameplay::LightComponent>();
    RegisterComponentPool<Gameplay::MovementComponent>();
}

void ComponentManager::DrawGUIAllComponents()
{

}
} // namespace Gameplay
