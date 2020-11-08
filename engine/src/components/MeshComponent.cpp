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
        m_material->CreateVAO(m_mesh->m_vbo, m_mesh->m_ebo);
}

void MeshComponent::SetMesh(Rendering::Mesh *mesh)
{
    m_mesh = mesh;
    if(m_material != nullptr) 
        m_material->CreateVAO(m_mesh->m_vbo, m_mesh->m_ebo);
}

void MeshComponent::SetMaterial(Rendering::Material *material)
{
    m_material = material;
    if(m_mesh != nullptr) 
        m_material->CreateVAO(m_mesh->m_vbo, m_mesh->m_ebo);
}

Engine::Geometry::Volume* MeshComponent::GetBoundingVolume()
{
    return m_mesh->m_boundingVolume->GetTransformed(GetModelMatrix());
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
    //TODO: Serialise the material properties as well!
    using namespace Utilities::JSON;

    auto serialiseTextures = [&]()
    {
        std::vector<JSONKeyValuePair> textures;
        for(auto tex : m_material->m_textures)
        {
            textures.push_back({tex.first, JSONValue::AsString(tex.second->GetResourceID().ToString())});
        }
        return JSONValue::AsObject(textures);
    };

    auto json = BaseComponent::Serialise();
    json->Members.push_back({ "mesh", JSONValue::AsString(m_meshAsset->ID.ToString()) });
    json->Members.push_back({ "meshOffset", JSONValue::AsArray({
        JSONValue::AsFloat(m_meshOffset.x), JSONValue::AsFloat(m_meshOffset.y), JSONValue::AsFloat(m_meshOffset.z)
    })});
    json->Members.push_back({ "shader", JSONValue::AsString(m_material->m_shader->m_name)});
    json->Members.push_back({ "textures", serialiseTextures() });
    return json;
}

void MeshComponent::Deserialise(std::shared_ptr<Utilities::JSON::JSONValue> json)
{
    BaseComponent::Deserialise(json);
    using namespace Assets;
    static auto assetManager = AssetManager::GetInstance();
    static auto renderer = Rendering::Renderer::GetInstance();
    auto textures = json->GetMember("textures");
    auto offset = json->GetMember("meshOffset")->Array;
    AssetID id = json->GetMember("mesh")->String;
    auto meshAsset = assetManager->GetAsset<MeshAsset>(id);
    UseMeshAsset(meshAsset);
    SetMeshOffset({offset[0]->Float, offset[1]->Float, offset[2]->Float});
    SetMaterial(renderer->GetShader(json->GetMember("shader")->String)->CreateMaterial());
    for(auto texMapping : textures->Members)
    {
        std::string textureName = texMapping.Key;
        AssetID textureID = texMapping.Value->String;
        ImageAsset* textureAsset = assetManager->GetAsset<ImageAsset>(textureID);
        m_material->UseTextureAsset(textureName, textureAsset);
        glm::vec3 f = vec3(0.24f);
        m_material->SetProperty<vec3>("PBRProperties", "F0", f);
    }
}

} // namespace Engine::Components

