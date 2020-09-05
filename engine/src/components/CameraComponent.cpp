#include "components/CameraComponent.hpp"
#include "core/GameObject.hpp"
#include "renderer/Renderer.hpp"
#include "renderer/WindowManager.hpp"
#include "renderer/Texture.hpp"
#include "utilities/Utilities.hpp"
#include "utilities/Printing.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <imgui.h>

#include <iostream>

using namespace std;

namespace Components
{

CameraComponent::CameraComponent() : BaseComponent("Camera Component")
{
    AspectRatio = Rendering::Renderer::GetInstance()->GetAspectRatio();
    SetMain();
}


void CameraComponent::Update(float deltaTime)
{
    AspectRatio = Rendering::Renderer::GetInstance()->GetAspectRatio();
    _camera.Position = gameObject->transform.position;
    _camera.ViewMatrix = gameObject->transform.GetViewMatrix();
    _camera.ProjectionMatrix = GetProjectionMatrix();
    _camera.BackgroundColour = BackgroundColour;
}

void CameraComponent::DrawInspectorGUI()
{
    ImGui::DragFloat("Field of View", &FieldOfView, 0.1f, 1.0f, 145.0f);
    ImGui::DragFloat("Near Plane", &NearPlane, 0.1f, 0.01f, 100.0f);
    ImGui::DragFloat("Far Plane", &FarPlane, 0.1f, 0.01f, 1000.0f);
    std::string ar = "Aspect ratio: " + std::to_string(AspectRatio);
    ImGui::Text(ar.c_str());
    
}

void CameraComponent::DrawGUI()
{
    static Rendering::Texture* texture = Utilities::ImportTexture("C:\\Users\\Svampex\\Documents\\Projects\\Graphics-Programming\\resources\\texture\\aa_beauty_and_the_sun.png");
    ImGui::Begin("Settings");
    ImGui::Image((ImTextureID)texture->GetID(), {100, 100});
    ImGui::TextColored(ImVec4(1,0,1,1), "Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::ColorPicker4("Clear colour", (float*)&BackgroundColour);
    ImGui::End();
}

void CameraComponent::SetMain()
{
    Rendering::Renderer::GetInstance()->SetMainCamera(&_camera);
}

glm::mat4 CameraComponent::GetViewMatrix()
{
    return gameObject->transform.GetViewMatrix();
}

glm::mat4 CameraComponent::GetProjectionMatrix()
{
    return glm::perspective(glm::radians(FieldOfView), AspectRatio, NearPlane, FarPlane);
}


Rendering::Ray CameraComponent::ScreenPointToRay(glm::vec2 point)
{
    auto wm = Rendering::WindowManager::GetInstance();
    auto size = wm->GetFrameBufferSize(wm->GetActiveWindow());
    glm::vec3 screenPoint1 = glm::vec3(point.x, size.y-point.y, 0.f);
    glm::vec3 screenPoint2 = glm::vec3(point.x, size.y-point.y, 1.f);

    glm::vec4 viewPort = glm::vec4(0.f,0.f,size.x,size.y);
    glm::vec3 origin = glm::unProject(screenPoint1, GetViewMatrix(), GetProjectionMatrix(), viewPort);
    glm::vec3 p = glm::unProject(screenPoint2, GetViewMatrix(), GetProjectionMatrix(), viewPort);

    Rendering::Ray ray = {origin, glm::normalize(p - origin)};
    return ray;
}

glm::vec3 CameraComponent::ColPlaneAt(Rendering::Ray r, float height)
{
    glm::vec3 result;
    if(glm::intersectRayPlane(r.Origin, r.Direction, glm::vec3(0.0f,height,0.0f), glm::vec3(0,1,0), result[0]))
    {
        return result;
    }
    return glm::vec3(0.0f);
}

Engine::Geometry::Frustum& CameraComponent::GetViewFrustum()
{
    glm::mat4 view = gameObject->transform.GetViewMatrix();
    glm::mat4 proj = GetProjectionMatrix();
    glm::mat4 m = proj * view;
    _viewFrustum.Planes[0] = -(m[3] + m[0]);
    _viewFrustum.Planes[1] = -(m[3] - m[0]);
    _viewFrustum.Planes[2] = -(m[3] + m[1]);
    _viewFrustum.Planes[3] = -(m[3] - m[1]);
    _viewFrustum.Planes[4] = -(m[3] + m[2]);
    _viewFrustum.Planes[5] = -(m[3] - m[2]);
    for(int i = 0; i < 6; ++i)
    {
        glm::vec4 plane = _viewFrustum.Planes[i];
        float mag = glm::length(glm::vec3(plane.x, plane.y, plane.z));
        _viewFrustum.Planes[i] *= 1.0f/mag;
    }
    return _viewFrustum;
}

}