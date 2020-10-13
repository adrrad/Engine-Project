#include "components/CameraComponent.hpp"
#include "core/GameObject.hpp"
#include "rendering/Renderer.hpp"
#include "platform/WindowManager.hpp"
#include "rendering/Texture.hpp"
#include "utilities/Utilities.hpp"
#include "utilities/Printing.hpp"


#include "editor/gui/DropField.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <imgui.h>

#include <iostream>

using namespace std;

namespace Engine::Components
{

CameraComponent* CameraComponent::MainCamera;

CameraComponent::CameraComponent()
{

}

void CameraComponent::Start()
{
    AspectRatio = Rendering::Renderer::GetInstance()->GetAspectRatio();
    SetMain();
    FieldOfView = 45.0f;
    AspectRatio = 1.0f;
    NearPlane = 0.1f;
    FarPlane = 1000.0f;
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
    ImGui::DragFloat("Far Plane", &FarPlane, 0.1f, 0.01f, 2000.0f);
    std::string ar = "Aspect ratio: " + std::to_string(AspectRatio);
    ImGui::Text(ar.c_str());
    ImGui::TextColored(ImVec4(1,0,1,1), "Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    

    auto skyboxGetter = [&]()
    {  
        if(m_skyboxCubemap) 
        {
            AssetID id = m_skyboxCubemap->GetResourceID();
            return Assets::AssetManager::GetInstance()->GetAsset<Assets::CubemapAsset>(id);
        }
        return (Engine::Assets::CubemapAsset *)nullptr;
    };
    auto skyboxSetter = [&](Assets::CubemapAsset* cubemap)
    {
        SetSkybox(cubemap);
    };
    Editor::DropField<Assets::CubemapAsset>("Skybox", skyboxGetter, skyboxSetter);
}

void CameraComponent::DrawGUI()
{
    ImGui::Begin("Settings");
    ImGui::TextColored(ImVec4(1,0,1,1), "Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::ColorPicker4("Clear colour", (float*)&BackgroundColour);
    ImGui::End();
}

void CameraComponent::SetMain()
{
    Rendering::Renderer::GetInstance()->SetMainCamera(&_camera);
    MainCamera = this;
}

void CameraComponent::SetSkybox(Assets::CubemapAsset* cubemap)
{
    m_skyboxCubemap = Rendering::Renderer::GetInstance()->GetCubemap(cubemap->ID);
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
    auto wm = Platform::WindowManager::GetInstance();
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
    glm::mat4 view = GetViewMatrix();
    glm::mat4 proj = GetProjectionMatrix();
    glm::mat4 m = proj * view;
    // _viewFrustum.Planes[0] = -(m[3] + m[0]);
    // _viewFrustum.Planes[1] = -(m[3] - m[0]);
    // _viewFrustum.Planes[2] = -(m[3] + m[1]);
    // _viewFrustum.Planes[3] = -(m[3] - m[1]);
    // _viewFrustum.Planes[4] = -(m[3] + m[2]);
    // _viewFrustum.Planes[5] = -(m[3] - m[2]);
    //https://www.reddit.com/r/gamedev/comments/xj47t/does_glm_support_frustum_plane_extraction/
    _viewFrustum.Planes[0].x = m[0][3] + m[0][0];
    _viewFrustum.Planes[0].y = m[1][3] + m[1][0];
    _viewFrustum.Planes[0].z = m[2][3] + m[2][0];
    _viewFrustum.Planes[0].w = m[3][3] + m[3][0];

    _viewFrustum.Planes[1].x = m[0][3] - m[0][0];
    _viewFrustum.Planes[1].y = m[1][3] - m[1][0];
    _viewFrustum.Planes[1].z = m[2][3] - m[2][0];
    _viewFrustum.Planes[1].w = m[3][3] - m[3][0];

    _viewFrustum.Planes[2].x = m[0][3] - m[0][1];
    _viewFrustum.Planes[2].y = m[1][3] - m[1][1];
    _viewFrustum.Planes[2].z = m[2][3] - m[2][1];
    _viewFrustum.Planes[2].w = m[3][3] - m[3][1];

    _viewFrustum.Planes[3].x = m[0][3] + m[0][1];
    _viewFrustum.Planes[3].y = m[1][3] + m[1][1];
    _viewFrustum.Planes[3].z = m[2][3] + m[2][1];
    _viewFrustum.Planes[3].w = m[3][3] + m[3][1];

    _viewFrustum.Planes[4].x = m[0][2];
    _viewFrustum.Planes[4].y = m[1][2];
    _viewFrustum.Planes[4].z = m[2][2];
    _viewFrustum.Planes[4].w = m[3][2];

    _viewFrustum.Planes[5].x = m[0][3] - m[0][2];
    _viewFrustum.Planes[5].y = m[1][3] - m[1][2];
    _viewFrustum.Planes[5].z = m[2][3] - m[2][2];
    _viewFrustum.Planes[5].w = m[3][3] - m[3][2];

    // for(int i = 0; i < 6; ++i)
    // {
    //     glm::vec4 plane = _viewFrustum.Planes[i];
    //     float mag = glm::length(glm::vec3(plane.x, plane.y, plane.z));
    //     _viewFrustum.Planes[i] *= 1.0f/mag;
    // }
    return _viewFrustum;
}

CameraComponent* CameraComponent::GetMainCamera()
{
    return MainCamera;
}

}