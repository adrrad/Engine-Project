#include "components/BaseComponent.hpp"
#include "renderer/SceneObject.hpp"

namespace Components
{

void BaseComponent::SetSceneObject(Rendering::SceneObject* sceneObject)
{
    this->sceneObject = sceneObject;
}

BaseComponent::BaseComponent(Rendering::SceneObject* sceneObject)
{
    this->sceneObject = sceneObject;
}

} // namespace Components
