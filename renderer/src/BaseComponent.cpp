#include "components/BaseComponent.hpp"
#include "renderer/SceneObject.hpp"

namespace Components
{

BaseComponent::BaseComponent(Rendering::SceneObject* sceneObject)
{
    this->sceneObject = sceneObject;
}


} // namespace Components
