#include "core/GameObject.hpp"

namespace Rendering
{

void GameObject::Update(float deltaTime)
{
    for(auto comp : _components)
    {
        comp->Update(deltaTime);
    }
}


} // namespace Rendering




