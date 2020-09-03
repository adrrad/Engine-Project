#include "components/MeshComponent.hpp"

#include "renderer/Renderer.hpp"

#include <glad/glad.h>
#include <imgui.h>

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

MeshComponent::MeshComponent()
{

}

void MeshComponent::Update(float deltaTime)
{

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

