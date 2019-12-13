#include "components/MeshComponent.hpp"

#include <glad/glad.h>
#include <imgui.h>

namespace Components
{

MeshComponent::MeshComponent()
{

}

void MeshComponent::Update(float deltaTime)
{

}

void MeshComponent::DrawGUI()
{
    if(_material == nullptr) return;
    ImGui::Begin("Settings");
    ImGui::TextColored(ImVec4(1,0,1,1), "Material Properties");
    for(uint32_t uniformIndex = 0; uniformIndex < _material->GetUniformCount(); uniformIndex++)
    {
        auto uniform = _material->GetUniform(uniformIndex);
        switch(uniform->Type)
        {
            case GL_FLOAT:
            uniform->f = _material->_shader->GetFloat(uniform->Name);
            ImGui::DragFloat(uniform->Name.c_str(), &uniform->f, 0.1f);
            break;
            case GL_FLOAT_VEC2:
            uniform->f2 = _material->_shader->GetVec2(uniform->Name);
            ImGui::DragFloat2(uniform->Name.c_str(), &uniform->f, 0.05f, 0.0f, 1.0f);
            break;
            case GL_FLOAT_VEC3:
            uniform->f3 = _material->_shader->GetVec3(uniform->Name);
            ImGui::DragFloat3(uniform->Name.c_str(), &uniform->f, 0.05f, 0.0f, 1.0f);
            break;
            case GL_FLOAT_VEC4:
            uniform->f4 = _material->_shader->GetVec4(uniform->Name);
            ImGui::DragFloat4(uniform->Name.c_str(), &uniform->f, 0.05f, 0.0f, 1.0f);
            break;
            default:
            break;
        }
    }
    ImGui::End();
}

void MeshComponent::SetMesh(Rendering::Mesh *mesh)
{
    _mesh = mesh;
}

void MeshComponent::SetMaterial(Rendering::Material *material)
{
    _material = material;
}


} // namespace Components

