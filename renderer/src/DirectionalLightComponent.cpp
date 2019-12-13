#include "components/DirectionalLightComponent.hpp"
#include "renderer/Renderer.hpp"
#include "imgui.h"

using namespace Rendering;
using namespace glm;
namespace Components
{

DirectionalLightComponent::DirectionalLightComponent()
{
    Renderer::GetInstance()->SetDirectionalLight(&this->_directionalLight);
    
}

void DirectionalLightComponent::Update(float deltaTime)
{
    _directionalLight.Direction = sceneObject->transform.GetDirection();
    //Renderer::GetInstance()->SetDirectionalLight(&this->_directionalLight);

    if(_debugDraw) DebugDraw();
}


void DirectionalLightComponent::SetColour(glm::vec4 colour)
{
    _colour = colour;
    _directionalLight.Colour = _colour;
}


void DirectionalLightComponent::SetDebugDrawDirectionEnabled(bool enabled)
{
    _debugDraw = enabled;
}

void DirectionalLightComponent::DebugDraw()
{
    LineSegment ls;
    ls.Vertices.push_back(vec3(0.0f, 0.0f, 0.0f));
    ls.Vertices.push_back(vec3(0.0f, 0.0f, 1.0f));
    ls.Transformation = sceneObject->transform.GetModelMatrix();
    Renderer::GetInstance()->DrawLineSegment(ls);
    ls.Vertices.clear();
    ls.Vertices.push_back(vec3(0.0f, 0.0f, 1.0f));
    ls.Vertices.push_back(vec3(0.0f, 0.1f, 0.8f));
    Renderer::GetInstance()->DrawLineSegment(ls);
    ls.Vertices.clear();
    ls.Vertices.push_back(vec3(0.0f, 0.0f, 1.0f));
    ls.Vertices.push_back(vec3(0.0f, -0.1f, 0.8f));
    Renderer::GetInstance()->DrawLineSegment(ls);
    ls.Vertices.clear();
    ls.Vertices.push_back(vec3(0.0f, 0.0f, 1.0f));
    ls.Vertices.push_back(vec3(-0.1f, 0.0f, 0.8f));
    Renderer::GetInstance()->DrawLineSegment(ls);
    ls.Vertices.clear();
    ls.Vertices.push_back(vec3(0.0f, 0.0f, 1.0f));
    ls.Vertices.push_back(vec3(0.1f, 0.0f, 0.8f));
    Renderer::GetInstance()->DrawLineSegment(ls);
}

void DirectionalLightComponent::DrawGUI()
{
ImGui::Begin("Settings");
    ImGui::TextColored(ImVec4(1,0,1,1), "Directional Light");
    ImGui::DragFloat3("Rotation", &sceneObject->transform.rotation[0], 0.5f, 0.0f, 360.0f);
ImGui::End();
}

} // namespace Components

