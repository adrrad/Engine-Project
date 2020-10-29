#include "components/MeshComponent.hpp"

#include "core/GameObject.hpp"
#include "rendering/Renderer.hpp"
#include "geometry/AxisAlignedBox.hpp"
#include "rendering/RenderingStructs.hpp"

#include "editor/gui/DropField.hpp"


#include <glad/glad.h>
#include <imgui.h>

using namespace Engine::Geometry;
using namespace Engine::Rendering;
using namespace glm;


namespace Engine::Components
{

MeshComponent::MeshComponent()
{
    m_meshOffset = vec3(0.0f, 0.0f, 0.0f);
}

void MeshComponent::DrawBB()
{
    AxisAlignedBox* box = (AxisAlignedBox*)GetBoundingVolume();

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

void MeshComponent::Start()
{

}

void MeshComponent::Update(float deltaTime)
{
    if(DrawBoundingBox) DrawBB();
}

void MeshComponent::DrawInspectorGUI()
{
    ImGui::DragFloat3("Mesh offset", &m_meshOffset[0], 0.1f);
    if(Editor::DropField("Mesh", m_meshAsset))
    {
       UseMeshAsset(m_meshAsset);
    }
}

void MeshComponent::UseMeshAsset(Assets::MeshAsset* asset)
{
    m_meshAsset = asset;
    m_mesh = Renderer::GetInstance()->GetMesh(asset->ID);
    if(m_material != nullptr) 
        m_material->CreateVAO(m_mesh->_vbo, m_mesh->_ebo);
}

void MeshComponent::SetMesh(Rendering::Mesh *mesh)
{
    m_mesh = mesh;
    if(m_material != nullptr) 
        m_material->CreateVAO(m_mesh->_vbo, m_mesh->_ebo);
}

void MeshComponent::SetMaterial(Rendering::Material *material)
{
    m_material = material;
    if(m_mesh != nullptr) 
        m_material->CreateVAO(m_mesh->_vbo, m_mesh->_ebo);
}

Engine::Geometry::Volume* MeshComponent::GetBoundingVolume()
{
    return m_mesh->_boundingVolume->GetTransformed(GetModelMatrix());
}

void MeshComponent::SetMeshOffset(glm::vec3 offset)
{
    m_meshOffset = offset;
}

glm::mat4 MeshComponent::GetModelMatrix()
{
    return gameObject->transform.GetModelMatrix(true) * mat4(vec4(1,0,0,0), vec4(0,1,0,0), vec4(0,0,1,0), vec4(m_meshOffset,1));
}

std::shared_ptr<Utilities::JSON::JSONValue> MeshComponent::Serialise()
{
    using namespace Utilities::JSON;
    
    auto json = BaseComponent::Serialise();
    json->Members.push_back({ "mesh", JSONValue::AsString(m_meshAsset->ID.ToString()) });
    return json;
}

void MeshComponent::Deserialise(std::shared_ptr<Utilities::JSON::JSONValue> json)
{
    BaseComponent::Deserialise(json);
    using namespace Assets;
    // UseMeshAsset(AssetManager::GetInstance()->GetAsset(json->))
}

} // namespace Engine::Components

