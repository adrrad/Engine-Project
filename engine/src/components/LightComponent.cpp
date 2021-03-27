#include "components/LightComponent.hpp"

#include "geometry/OrientedBox.hpp"
#include "rendering/Renderer.hpp"
#include "core/GameObject.hpp"
#include "imgui.h"

using namespace Engine::Rendering;
using namespace glm;
namespace Engine::Components
{

glm::mat4 LightComponent::ViewProjectionMatrix()
{
    if(m_type == LightType::DIRECTIONAL) return OrthographicProjectionMatrix() * gameObject->transform.GetViewMatrix();
    else return PerspectiveProjectionMatrix() * gameObject->transform.GetViewMatrix();
}

Geometry::Volume* LightComponent::GetLightVolume()
{
    float hu = (m_farPlane - m_nearPlane) * 0.5f;
    float hv = m_viewSize * 0.5f;
    float hw = m_viewSize * 0.5f;
    
    mat4 trs = gameObject->transform.GetModelMatrix();
    vec3 u = vec3(trs * vec4(0,0,-1,0));
    vec3 v = vec3(trs * vec4(1,0,0,0));
    vec3 w = vec3(trs * vec4(0,1,0,0));
    vec3 center = gameObject->transform.position + u * hu;
    return new Geometry::OrientedBox(center, u, v, w, hu, hv, hw);
}

void LightComponent::UpdateLight()
{
    switch (m_type)
    {
    case LightType::DIRECTIONAL:
        m_directionalLight->Direction = -gameObject->transform.Forward();
        m_directionalLight->ViewProjection = ViewProjectionMatrix();
        break;
    case LightType::POINT:
        m_pointLight->Position = gameObject->transform.GetGlobalPosition();
        break;
    default:
        break;
    }
}

LightComponent::LightComponent()
{
    m_directionalLight = Renderer::GetInstance()->GetNewDirectionalLight(&m_lightBuffer);
    m_shadowmap = Framebuffer::Create("shadowmap", 4096, 4096).WithDepthbuffer("depth").Build();
    SetColour(vec4(1.0f));
    SetType(LightType::DIRECTIONAL);
}

void LightComponent::Start()
{

}

void DrawViewFrustum(std::vector<vec3> v)
{
    LineSegment ls;
    ls.Colour = {1, 0, 0, 0};
    ls.Vertices = {v[0], v[1], v[2], v[3], v[0]};
    Renderer::GetInstance()->DrawLineSegment(ls);
    ls.Colour = {0, 1, 0, 0};
    ls.Vertices.clear();
    ls.Vertices = {v[4], v[5], v[6], v[7], v[4]};
    Renderer::GetInstance()->DrawLineSegment(ls);
    ls.Vertices.clear();
    ls.Vertices = {v[0], v[3], v[7], v[4], v[0]};
    Renderer::GetInstance()->DrawLineSegment(ls);
    ls.Vertices.clear();
    ls.Vertices = {v[1], v[2], v[6], v[5], v[1]};
    Renderer::GetInstance()->DrawLineSegment(ls);
}


void LightComponent::Update(float deltaTime)
{
    Geometry::OrientedBox* box = dynamic_cast<Geometry::OrientedBox*>(GetLightVolume());
    if(m_type == LightType::DIRECTIONAL) DrawViewFrustum(box->GetCorners());
    UpdateLight();
    if(m_debugDraw) DebugDraw();
    DebugDraw();
}


void LightComponent::SetColour(glm::vec4 colour)
{
    m_directionalLight->Colour = colour;
}

void LightComponent::SetType(LightType type)
{
    m_type = type;
    switch (m_type)
    {
    case LightType::DIRECTIONAL:
        if(m_directionalLight ==nullptr) m_directionalLight = Renderer::GetInstance()->GetNewDirectionalLight(&m_lightBuffer);
        break;
    case LightType::POINT:
        if(m_pointLight == nullptr) m_pointLight = Renderer::GetInstance()->GetNewPointLight(&m_lightBuffer);
        m_pointLight->Position = gameObject->transform.GetGlobalPosition();
        break;
    default:
        break;
    }
}

LightType LightComponent::GetType()
{
    return m_type;
}

Rendering::PointLight& LightComponent::PointLight()
{
    if(m_type == LightType::POINT) return *m_pointLight;
    throw "Light type is not point light!";
}

Rendering::DirectionalLight& LightComponent::DirectionalLight()
{
    if(m_type == LightType::DIRECTIONAL) return *m_directionalLight;
    throw "Light type is not directional light!";
}



void LightComponent::SetDebugDrawDirectionEnabled(bool enabled)
{
    m_debugDraw = enabled;
}

void LightComponent::DebugDraw()
{
    LineSegment ls;
    ls.Width = 5;
    float scale = 5.0f;
    mat4 M = gameObject->transform.GetModelMatrix(true);
    ls.Vertices.push_back(M * (vec4(0.0f, 0.0f, 0.0f, 1.0f)));
    ls.Vertices.push_back(M * (vec4(0.0f, 0.0f, 1.0f, 1.0f)));
    Renderer::GetInstance()->DrawLineSegment(ls);
    ls.Vertices.clear();
    ls.Vertices.push_back(M * (vec4(0.0f, 0.0f, 1.0f, 1.0f)));
    ls.Vertices.push_back(M * (vec4(0.0f, 0.1f, 0.8f, 1.0f)));
    Renderer::GetInstance()->DrawLineSegment(ls);
    ls.Vertices.clear();
    ls.Vertices.push_back(M * (vec4(0.0f, 0.0f, 1.0f, 1.0f) ));
    ls.Vertices.push_back(M * (vec4(0.0f, -0.1f, 0.8f, 1.0f)));
    Renderer::GetInstance()->DrawLineSegment(ls);
    ls.Vertices.clear();
    ls.Vertices.push_back(M * (vec4(0.0f, 0.0f, 1.0f, 1.0f) ));
    ls.Vertices.push_back(M * (vec4(-0.1f, 0.0f, 0.8f, 1.0f)));
    Renderer::GetInstance()->DrawLineSegment(ls);
    ls.Vertices.clear();
    ls.Vertices.push_back(M * (vec4(0.0f, 0.0f, 1.0f, 1.0f) ));
    ls.Vertices.push_back(M * (vec4(0.1f, 0.0f, 0.8f, 1.0f) ));
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
            ImGui::DragFloat4("Colour", &m_directionalLight->Colour[0], 0.05f, 0.0f);
            ImGui::TreePop();
        }
        break;
        case LightType::POINT:
        if(ImGui::TreeNode("Point Light Component"))
        {
            ImGui::DragFloat4("Colour", &m_pointLight->Colour[0], 0.05f, 0.0f);
            ImGui::DragFloat("Radius", &m_pointLight->Radius, 0.05f, 0.0f);
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
        ImGui::DragFloat4("Colour", &m_pointLight->Colour[0], 0.05f, 0.0f);
        ImGui::DragFloat("Radius", &m_pointLight->Radius, 0.05f, 0.0f);
    }
    else
    {
        ImGui::DragFloat3("Colour", &m_directionalLight->Colour[0], 0.05f);
        ImGui::Text("Shadowmap");
        ImGui::Image(ImTextureID(m_shadowmap->GetDepthBuffer("depth")->GetID()), {256, 256});
    }
}

std::shared_ptr<Utilities::JSON::JSONValue> LightComponent::Serialise()
{
    using namespace Utilities::JSON;
    auto json = BaseComponent::Serialise();
    // TODO: ADD JSON INT VALUES
    json->Members.push_back({ "type", JSONValue::AsFloat(float(m_type)) });
    glm::vec4 c;
    if(m_type == LightType::DIRECTIONAL) c = m_directionalLight->Colour;
    else if(m_type == LightType::POINT)
    {
        c = m_pointLight->Colour;
        json->Members.push_back({ "radius", JSONValue::AsFloat(m_pointLight->Radius)});
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
        m_directionalLight->Colour = { col[0]->Float, col[1]->Float, col[2]->Float, col[3]->Float };
    }
    else if(m_type == LightType::POINT)
    {
        m_pointLight->Colour = { col[0]->Float, col[1]->Float, col[2]->Float, col[3]->Float };
        m_pointLight->Radius = json->GetMember("radius")->Float;
    }
}

} // namespace Engine::Components

