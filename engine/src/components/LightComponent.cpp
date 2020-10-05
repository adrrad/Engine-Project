#include "components/LightComponent.hpp"
#include "rendering/Renderer.hpp"
#include "core/GameObject.hpp"
#include "imgui.h"

using namespace Engine::Rendering;
using namespace glm;
namespace Engine::Components
{

void LightComponent::UpdateLight()
{
    switch (_type)
    {
    case LightType::DIRECTIONAL:
        _directionalLight->Direction = gameObject->transform.GetDirection();
        break;
    case LightType::POINT:
        _pointLight->Position = gameObject->transform.GetGlobalPosition();
        break;
    default:
        break;
    }
}

LightComponent::LightComponent()
{
    SetType(LightType::DIRECTIONAL);
    SetColour(vec4(1.0f));
    ViewFrustum = new Engine::Geometry::AxisAlignedBox(vec3(0,0,0), vec3(10.0f));
}

void LightComponent::Start()
{

}

void LightComponent::Update(float deltaTime)
{
    UpdateLight();
    if(_debugDraw) DebugDraw();
}


void LightComponent::SetColour(glm::vec4 colour)
{
    _directionalLight->Colour = colour;
}

void LightComponent::SetType(LightType type)
{
    _type = type;
    switch (_type)
    {
    case LightType::DIRECTIONAL:
        Renderer::GetInstance()->SetDirectionalLight(_directionalLight);
        break;
    case LightType::POINT:
        if(_pointLight == nullptr) _pointLight = Renderer::GetInstance()->GetNewPointLight();
        _pointLight->Position = gameObject->transform.GetGlobalPosition();
        break;
    default:
        break;
    }
}

Rendering::PointLight& LightComponent::PointLight()
{
    if(_type == LightType::POINT) return *_pointLight;
    throw "Light type is not point light!";
}

Rendering::DirectionalLight& LightComponent::DirectionalLight()
{
    if(_type == LightType::DIRECTIONAL) return *_directionalLight;
    throw "Light type is not directional light!";
}



void LightComponent::SetDebugDrawDirectionEnabled(bool enabled)
{
    _debugDraw = enabled;
}

void LightComponent::DebugDraw()
{
    LineSegment ls;
    ls.Width = 10;
    float scale = 5.0f;
    mat4 M = gameObject->transform.GetModelMatrix();
    ls.Vertices.push_back(M * vec4(0.0f, 0.0f, 0.0f, 1.0f) * scale);
    ls.Vertices.push_back(M * vec4(0.0f, 0.0f, 1.0f, 1.0f) * scale);
    Renderer::GetInstance()->DrawLineSegment(ls);
    ls.Vertices.clear();
    ls.Vertices.push_back(M * vec4(0.0f, 0.0f, 1.0f, 1.0f) * scale);
    ls.Vertices.push_back(M * vec4(0.0f, 0.1f, 0.8f, 1.0f) * scale);
    Renderer::GetInstance()->DrawLineSegment(ls);
    ls.Vertices.clear();
    ls.Vertices.push_back(M * vec4(0.0f, 0.0f, 1.0f, 1.0f) * scale);
    ls.Vertices.push_back(M * vec4(0.0f, -0.1f, 0.8f, 1.0f) * scale);
    Renderer::GetInstance()->DrawLineSegment(ls);
    ls.Vertices.clear();
    ls.Vertices.push_back(M * vec4(0.0f, 0.0f, 1.0f, 1.0f) * scale);
    ls.Vertices.push_back(M * vec4(-0.1f, 0.0f, 0.8f, 1.0f) * scale);
    Renderer::GetInstance()->DrawLineSegment(ls);
    ls.Vertices.clear();
    ls.Vertices.push_back(M * vec4(0.0f, 0.0f, 1.0f, 1.0f) * scale);
    ls.Vertices.push_back(M * vec4(0.1f, 0.0f, 0.8f, 1.0f) * scale);
    Renderer::GetInstance()->DrawLineSegment(ls);
}

void LightComponent::DebugGUI()
{
    ImGui::PushID(this);
    switch(_type)
    {
        case LightType::DIRECTIONAL:
        if(ImGui::TreeNode("Directional Light Component"))
        {
            ImGui::DragFloat4("Colour", &_directionalLight->Colour[0], 0.05f, 0.0f);
            ImGui::TreePop();
        }
        break;
        case LightType::POINT:
        if(ImGui::TreeNode("Point Light Component"))
        {
            ImGui::DragFloat4("Colour", &_pointLight->Colour[0], 0.05f, 0.0f);
            ImGui::DragFloat("Radius", &_pointLight->Radius, 0.05f, 0.0f);
            ImGui::TreePop();
        }
        break;
    }
    ImGui::PopID();
}

void LightComponent::DrawInspectorGUI()
{
    if(_type == LightType::POINT)
    {
        ImGui::DragFloat3("Light Colour", &_pointLight->Colour[0], 0.05f);
    }
    else
    {
        ImGui::DragFloat3("Light Colour", &_directionalLight->Colour[0], 0.05f);
    }
    
}
} // namespace Engine::Components

