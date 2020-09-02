#include "core/GameObject.hpp"

namespace Engine::Core
{

void GameObject::Update(float deltaTime)
{
    for(auto comp : _components)
    {
        comp->Update(deltaTime);
    }
}


} // namespace Rendering




