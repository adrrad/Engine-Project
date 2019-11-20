#include "components/DirectionalLightComponent.hpp"
#include "renderer/Renderer.hpp"

using namespace Rendering;
using namespace glm;
namespace Components
{

DirectionalLightComponent::DirectionalLightComponent(Rendering::SceneObject* sceneObject)
: BaseComponent(sceneObject)
{
    Renderer::GetInstance()->SetDirectionalLight(&this->_directionalLight);
    
}

void DirectionalLightComponent::Update(float deltaTime)
{
    sceneObject->transform.rotation.x += 25.0f*deltaTime;
    _directionalLight.Direction = sceneObject->transform.GetDirection();
    Renderer::GetInstance()->SetDirectionalLight(&this->_directionalLight);
    LineSegment ls;
    ls.Vertices.push_back(vec3(0.0f, 0.0f, 0.0f));
    ls.Vertices.push_back(vec3(0.0f, 0.0f, 1.0f));
    ls.Transformation = sceneObject->transform.GetModelMatrix();

    Renderer::GetInstance()->DrawLineSegment(ls);
}


void DirectionalLightComponent::SetColour(glm::vec4 colour)
{
    _colour = colour;
    _directionalLight.Colour = _colour;
}

} // namespace Components

