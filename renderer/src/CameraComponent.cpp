#include "components/CameraComponent.hpp"
#include "renderer/SceneObject.hpp"
#include "renderer/Renderer.hpp"

#include <glm/gtc/matrix_transform.hpp>

namespace Components
{

CameraComponent::CameraComponent(Rendering::SceneObject* sceneObject) 
: BaseComponent(sceneObject)
{
    AspectRatio = Rendering::Renderer::GetInstance()->GetAspectRatio();
}


void CameraComponent::Update(float deltaTime)
{
    _camera.ViewMatrix = sceneObject->transform.GetViewMatrix();
    _camera.ProjectionMatrix = glm::perspective(FieldOfView, AspectRatio, NearPlane, FarPlane);
}


void CameraComponent::SetMain()
{
    Rendering::Renderer::GetInstance()->SetMainCamera(&_camera);
}

}