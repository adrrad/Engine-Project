#include "components/MeshComponent.hpp"

#include "renderer/Renderer.hpp"
#include "geometry/AxisAlignedBox.hpp"
#include "renderer/RenderingStructs.hpp"
#include <glad/glad.h>
#include <imgui.h>

using namespace Engine::Geometry;
using namespace Rendering;
using namespace glm;


namespace Components
{

void MeshComponent::DebugGUI()
{
    if(ImGui::TreeNode("Mesh Component"))
    {
        if(ImGui::Button("Recompile Shader"))
        {
            _material->_shader->RecompileShader();
            Rendering::Renderer::GetInstance()->InvalidateRenderpass();
        }
        ImGui::TextColored(ImVec4(1,0,1,1), "Material");
        ImGui::TreePop();
    }
}

MeshComponent::MeshComponent() : BaseComponent("Mesh Component")
{

}

void MeshComponent::DrawBB()
{
    AxisAlignedBox* box = (AxisAlignedBox*)_mesh->_boundingVolume->GetTransformed(gameObject->transform.GetModelMatrix(true));

    vec3 min, max;
    vec3 v1, v2, v3, v4;
    vec3 v5, v6, v7, v8;
    min = box->Min;
    max = box->Max;
    v1 = min;
    v2 = vec3(max.x, min.y, min.z);
    v3 = vec3(max.x, max.y, min.z);
    v4 = vec3(min.x, max.y, min.z);

    v5 = vec3(min.x, min.y, max.z);
    v6 = vec3(max.x, min.y, max.z);
    v7 = vec3(max.x, max.y, max.z);
    v8 = vec3(min.x, max.y, max.z);

    LineSegment ls;
    ls.Colour = {1, 1, 0, 0};
    ls.Vertices = {v1, v2, v3, v4, v1};
    Renderer::GetInstance()->DrawLineSegment(ls);
    ls.Vertices.clear();
    ls.Vertices = {v5, v6, v7, v8, v5};
    Renderer::GetInstance()->DrawLineSegment(ls);
    ls.Vertices.clear();
    ls.Vertices = {v1, v4, v8, v5, v1};
    Renderer::GetInstance()->DrawLineSegment(ls);
    ls.Vertices.clear();
    ls.Vertices = {v2, v3, v7, v6, v2};
    Renderer::GetInstance()->DrawLineSegment(ls);
}

void MeshComponent::Update(float deltaTime)
{
    if(DrawBoundingBox) DrawBB();
}

void MeshComponent::DrawGUI()
{
    ImGui::Begin("Settings");
    ImGui::PushID(this);
    ImGui::PopID();
    ImGui::End();
}

void MeshComponent::SetMesh(Rendering::Mesh *mesh)
{
    _mesh = mesh;
    if(_material != nullptr) 
        _material->CreateVAO(_mesh->_vbo, _mesh->_ebo);
}

void MeshComponent::SetMaterial(Rendering::Material *material)
{
    _material = material;
    if(_mesh != nullptr) 
        _material->CreateVAO(_mesh->_vbo, _mesh->_ebo);
}

void MeshComponent::SetTexture(Rendering::Texture *texture)
{
    _texture = texture;
}

Engine::Geometry::Volume* MeshComponent::GetBoundingVolume()
{
    return _mesh->_boundingVolume->GetTransformed(gameObject->transform.GetModelMatrix(true));
}


} // namespace Components

