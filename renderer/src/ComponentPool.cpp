#include "components/ComponentPool.hpp"


namespace Components
{
std::vector<IComponentPool*> ComponentManager::_pools;
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
