#include "components/CameraComponent.hpp"
#include "gameplay/GameObject.hpp"
#include "rendering/Renderer.hpp"
#include "platform/WindowManager.hpp"
#include "rendering/Texture.hpp"
#include "utilities/Utilities.hpp"
#include "utilities/Printing.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <imgui.h>

#include <iostream>

using namespace std;

namespace Gameplay
{

CameraComponent* CameraComponent::MainCamera;

CameraComponent::CameraComponent()
{
    m_camera = Rendering::Renderer::GetInstance()->GetNewCamera(&m_cameraBuffer);
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
    m_camera->Position = gameObject->transform.position;
    m_camera->ViewMatrix = gameObject->transform.GetViewMatrix();
    m_camera->ProjectionMatrix = GetProjectionMatrix();
    m_camera->BackgroundColour = BackgroundColour;
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

Geometry::Volume* CameraComponent::GetViewVolume()
{
    glm::mat4 view = GetViewMatrix();
    glm::mat4 proj = GetProjectionMatrix();
    glm::mat4 m = proj * view;
    return new Geometry::Frustum(m);
}

CameraComponent* CameraComponent::GetMainCamera()
{
    return MainCamera;
}

std::shared_ptr<Utilities::JSON::JSONValue> CameraComponent::Serialise()
{
    using namespace Utilities::JSON;
    glm::vec4& c = BackgroundColour;
    auto json = BaseComponent::Serialise();
    json->Members.push_back({ "fov", JSONValue::AsFloat(FieldOfView)});
    json->Members.push_back({ "aspectRatio", JSONValue::AsFloat(AspectRatio)});
    json->Members.push_back({ "nearPlane", JSONValue::AsFloat(NearPlane)});
    json->Members.push_back({ "farPlane", JSONValue::AsFloat(FarPlane)});
    json->Members.push_back({ "background", JSONValue::AsArray({ JSONValue::AsFloat(c.r), JSONValue::AsFloat(c.g), JSONValue::AsFloat(c.b), JSONValue::AsFloat(c.a)})});
    return json;
}

void CameraComponent::Deserialise(std::shared_ptr<Utilities::JSON::JSONValue> json)
{
    BaseComponent::Deserialise(json);
    auto col = json->GetMember("background")->Array;
    FieldOfView = json->GetMember("fov")->Float;
    AspectRatio = json->GetMember("aspectRatio")->Float;
    NearPlane = json->GetMember("nearPlane")->Float;
    FarPlane = json->GetMember("farPlane")->Float;
    BackgroundColour = { col[0]->Float, col[1]->Float, col[2]->Float, col[3]->Float };
}

}