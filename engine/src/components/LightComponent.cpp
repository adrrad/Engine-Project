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
    switch (m_type)
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
    m_type = type;
    switch (m_type)
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
    if(m_type == LightType::POINT) return *_pointLight;
    throw "Light type is not point light!";
}

Rendering::DirectionalLight& LightComponent::DirectionalLight()
{
    if(m_type == LightType::DIRECTIONAL) return *_directionalLight;
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
    switch(m_type)
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
    if(m_type == LightType::POINT)
    {         
        ImGui::DragFloat4("Colour", &_pointLight->Colour[0], 0.05f, 0.0f);
        ImGui::DragFloat("Radius", &_pointLight->Radius, 0.05f, 0.0f);
    }
    else
    {
        ImGui::DragFloat3("Light Colour", &_directionalLight->Colour[0], 0.05f);
    }
    
}

std::shared_ptr<Utilities::JSON::JSONValue> LightComponent::Serialise()
{
    using namespace Utilities::JSON;
    auto json = BaseComponent::Serialise();
    // TODO: ADD JSON INT VALUES
    json->Members.push_back({ "type", JSONValue::AsFloat(float(m_type)) });
    glm::vec4 c;
    if(m_type == LightType::DIRECTIONAL) c = _directionalLight->Colour;
    else if(m_type == LightType::POINT)
    {
        c = _pointLight->Colour;
        json->Members.push_back({ "radius", JSONValue::AsFloat(_pointLight->Radius)});
    }
    json->Members.push_back({ "colour", JSONValue::AsArray({
        JSONValue::AsFloat(c.r), 
        JSONValue::AsFloat(c.g), 
        JSONValue::AsFloat(c.b), 
        JSONValue::AsFloat(c.a)})});
    return json;
}

void LightComponent::Deserialise(std::shared_ptr<Utilities::JSON::JSONValue> json)
{
    BaseComponent::Deserialise(json);
    SetType(LightType(json->GetMember("type")->Float)); // TODO: ADD JSON INT VALUES
    auto col = json->GetMember("colour")->Array;
    if(m_type == LightType::DIRECTIONAL) 
    {
        _directionalLight->Colour = { col[0]->Float, col[1]->Float, col[2]->Float, col[3]->Float };
    }
    else if(m_type == LightType::POINT)
    {
        _pointLight->Colour = { col[0]->Float, col[1]->Float, col[2]->Float, col[3]->Float };
        _pointLight->Radius = json->GetMember("radius")->Float;
    }
}

} // namespace Engine::Components

