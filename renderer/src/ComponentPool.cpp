#include "components/ComponentPool.hpp"


namespace Components
{
std::vector<IComponentPool*> ComponentManager::_pools;
std::unordered_map<const char*, IComponentPool*> ComponentManager::_mapping;
void ComponentManager::UpdateAllComponents(float deltaTime)
{
    for(auto pool : _pools)
    {
        pool->Update(deltaTime);
    }
}


void ComponentManager::DrawGUIAllComponents()
{
    for(auto pool : _pools)
    {
        pool->DrawGUI();
    }
}
} // namespace Components
