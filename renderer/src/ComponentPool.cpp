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
} // namespace Components
