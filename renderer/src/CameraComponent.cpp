#include "components/CameraComponent.hpp"
#include "renderer/SceneObject.hpp"
#include "renderer/Renderer.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <imgui.h>

#include <iostream>

using namespace std;

namespace Components
{

CameraComponent::CameraComponent()
{
    AspectRatio = Rendering::Renderer::GetInstance()->GetAspectRatio();
}


void CameraComponent::Update(float deltaTime)
{
    _camera.ViewMatrix = sceneObject->transform.GetViewMatrix();
    _camera.ProjectionMatrix = glm::perspective(FieldOfView, AspectRatio, NearPlane, FarPlane);
}

void CameraComponent::DrawGUI()
{
    ImGui::Begin("Settings");
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::End();
}

void CameraComponent::SetMain()
{
    Rendering::Renderer::GetInstance()->SetMainCamera(&_camera);
}

}