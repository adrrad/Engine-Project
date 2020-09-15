#include "components/ComponentManager.hpp"


namespace Engine::Components
{
ComponentManager* ComponentManager::instance;

std::vector<IComponentPool*> ComponentManager::_pools;
std::unordered_map<const char*, IComponentPool*> ComponentManager::_mapping;

ComponentManager* ComponentManager::GetInstance()
{
    if(instance == nullptr) instance = new ComponentManager();
    return instance;
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
