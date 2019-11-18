#include "components/DirectionalLightComponent.hpp"
#include "renderer/Renderer.hpp"

namespace Components
{

DirectionalLightComponent::DirectionalLightComponent(Rendering::SceneObject* sceneObject)
: BaseComponent(sceneObject)
{
    Rendering::Renderer::GetInstance()->SetDirectionalLight(&this->_directionalLight);
    
}

void DirectionalLightComponent::Update(float deltaTime)
{
    sceneObject->transform.rotation.x += 25.0f*deltaTime;
    _directionalLight.Direction = sceneObject->transform.GetDirection();
    Rendering::Renderer::GetInstance()->SetDirectionalLight(&this->_directionalLight);
}


void DirectionalLightComponent::SetColour(glm::vec4 colour)
{
    _colour = colour;
    _directionalLight.Colour = _colour;
}

} // namespace Components

