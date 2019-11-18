#include "renderer/SceneObject.hpp"

namespace Rendering
{

void SceneObject::Update(float deltaTime)
{
    for(auto comp : _components)
    {
        comp->Update(deltaTime);
    }
}


} // namespace Rendering




