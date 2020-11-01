#include "components/ComponentManager.hpp"
#include "core/GameObject.hpp"
#include "Exceptions.hpp"

namespace Engine::Components
{
ComponentManager* ComponentManager::instance;

std::vector<IComponentPool*> ComponentManager::_pools;
std::unordered_map<std::string, IComponentPool*> ComponentManager::_mapping;

ComponentManager* ComponentManager::GetInstance()
{
    if(instance == nullptr) instance = new ComponentManager();
    return instance;
}

BaseComponent* ComponentManager::DeserialiseComponent(Core::GameObject* go, std::shared_ptr<Utilities::JSON::JSONValue> json)
{
    std::string componentName = json->GetMember("name")->String;
    if(_mapping.contains(componentName))
    {
        BaseComponent* comp = _mapping[componentName]->AllocateNewComponent();
        comp->gameObject = go;
        comp->Deserialise(json);
        go->m_components.insert({componentName, comp});
        return comp;
    }
    throw EngineException("Could not deserialise component '" + componentName + "'");
}

void ComponentManager::Start()
{
    for(auto pool : _pools)
    {
        pool->Start();
    }
}

void ComponentManager::Update(float deltaTime)
{
    for(auto pool : _pools)
    {
        pool->Update(deltaTime);
    }
}


void ComponentManager::DrawGUIAllComponents()
{
    // for(auto pool : _pools)
    // {
    //     pool->DrawGUI();
    // }
}
} // namespace Engine::Components
