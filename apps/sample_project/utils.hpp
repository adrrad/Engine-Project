
#include "rendering/Renderer.hpp"
#include "rendering/Mesh.hpp"
#include "core/Scene.hpp"
#include "rendering/Renderpass.hpp"
#include "utilities/Utilities.hpp"

#include "components/CameraComponent.hpp"
#include "components/MovementComponent.hpp"
#include "components/LightComponent.hpp"
#include "components/MeshComponent.hpp"
#include "components/InspectorCameraComponent.hpp"

#include "editor/SceneInspector.hpp"
#include "editor/EditorCore.hpp"

#include "acceleration/AABSPTree.hpp"
#include "acceleration/Octree.hpp"
#include "geometry/Sphere.hpp"
#include <iostream>

#include "acceleration/MeshProcessor.hpp"
#include "geometry/Point.hpp"
#include "physics/PhysicsManager.hpp"
#include "physics/RigidBody.hpp"
#include "components/RigidBodyComponent.hpp"
#include "utilities/Printing.hpp"
#include "utilities/MathUtils.hpp"

#include "core/EngineSubsystem.hpp"
#include "core/EngineCore.hpp"

#include "platform/io/File.hpp"
#include "platform/io/Directory.hpp"
#include "platform/io/Filesystem.hpp"

#include "utilities/json/JSON.hpp"
#include "assets/Asset.hpp"
#include "assets/AssetManager.hpp"
#include "assets/MeshAsset.hpp"
#include "assets/importing/MeshImporter.hpp"

#include "utilities/IdentifierGenerator.hpp"

#include <glm/glm.hpp>


inline GameObject* CreateSphere(vec3 position, Shader* shader)
{
    static Texture* albedo =   Utilities::ImportTexture(GetAbsoluteResourcesPath("\\PBR_materials\\[4K]Tiles58\\Tiles58_col.jpg"), GL_TEXTURE_2D);
    static Texture* metallic = Utilities::ImportTexture(GetAbsoluteResourcesPath("\\PBR_materials\\[4K]Tiles58\\Tiles58_met.jpg"), GL_TEXTURE_2D);
    static Texture* roughness =Utilities::ImportTexture(GetAbsoluteResourcesPath("\\PBR_materials\\[4K]Tiles58\\Tiles58_rgh.jpg"), GL_TEXTURE_2D);
    static Texture* normal =   Utilities::ImportTexture(GetAbsoluteResourcesPath("\\PBR_materials\\[4K]Tiles58\\Tiles58_nrm.jpg"), GL_TEXTURE_2D);
    
    Platform::IO::File objfile(RESOURCES_DIR+string("models/sphere.obj"));
    static auto whatever = Assets::Importing::MeshImporter::ImportMesh(&objfile);
    
    static Mesh* sphereMesh = new Mesh(whatever);//Mesh::GetSphere();
    
    GameObject* sphere = scene.InstantiateGameObject();
    sphere->Name = "Sphere";
    sphere->transform.position = position;

    auto mp = sphere->AddComponent<MeshComponent>();
    Material* mat = shader->CreateMaterial();
    vec3 f = vec3(0.24f);
    mat->SetProperty<vec3>("PBRProperties", "F0", f);
    mat->SetTexture("textures.albedo", albedo);
    mat->SetTexture("textures.metallic", metallic);
    mat->SetTexture("textures.roughness", roughness);
    mat->SetTexture("textures.normal", normal);
    mp->SetMesh(sphereMesh);
    mp->SetMaterial(mat);
    return sphere;
}

inline GameObject* CreateCube(vec3 position, Shader* shader)
{
    static Texture* albedo =   Utilities::ImportTexture(GetAbsoluteResourcesPath("\\PBR_materials\\[4K]Tiles58\\Tiles58_col.jpg"), GL_TEXTURE_2D);
    static Texture* metallic = Utilities::ImportTexture(GetAbsoluteResourcesPath("\\PBR_materials\\[4K]Tiles58\\Tiles58_met.jpg"), GL_TEXTURE_2D);
    static Texture* roughness =Utilities::ImportTexture(GetAbsoluteResourcesPath("\\PBR_materials\\[4K]Tiles58\\Tiles58_rgh.jpg"), GL_TEXTURE_2D);
    static Texture* normal =   Utilities::ImportTexture(GetAbsoluteResourcesPath("\\PBR_materials\\[4K]Tiles58\\Tiles58_nrm.jpg"), GL_TEXTURE_2D);
    static Mesh* sphereMesh =  Mesh::GetCube();
    
    GameObject* sphere = scene.InstantiateGameObject();
    sphere->Name = "Sphere";
    sphere->transform.position = position;

    auto mp = sphere->AddComponent<MeshComponent>();
    Material* mat = shader->CreateMaterial();
    vec3 f = vec3(0.24f);
    mat->SetProperty<vec3>("PBRProperties", "F0", f);
    mat->SetTexture("textures.albedo", albedo);
    mat->SetTexture("textures.metallic", metallic);
    mat->SetTexture("textures.roughness", roughness);
    mat->SetTexture("textures.normal", normal);
    mp->SetMesh(sphereMesh);
    mp->SetMaterial(mat);
    return sphere;
}

inline GameObject* CreateQuad(vec3 position, Shader* shader)
{
    static Texture* albedo =   Utilities::ImportTexture(GetAbsoluteResourcesPath("\\PBR_materials\\[4K]Tiles58\\Tiles58_col.jpg"), GL_TEXTURE_2D);
    static Texture* metallic = Utilities::ImportTexture(GetAbsoluteResourcesPath("\\PBR_materials\\[4K]Tiles58\\Tiles58_met.jpg"), GL_TEXTURE_2D);
    static Texture* roughness =Utilities::ImportTexture(GetAbsoluteResourcesPath("\\PBR_materials\\[4K]Tiles58\\Tiles58_rgh.jpg"), GL_TEXTURE_2D);
    static Texture* normal =   Utilities::ImportTexture(GetAbsoluteResourcesPath("\\PBR_materials\\[4K]Tiles58\\Tiles58_nrm.jpg"), GL_TEXTURE_2D);
    static Mesh* sphereMesh =  Mesh::GetQuad();

    GameObject* sphere = scene.InstantiateGameObject();
    sphere->Name = "Quad";
    sphere->transform.position = position;

    auto mp = sphere->AddComponent<MeshComponent>();
    Material* mat = shader->CreateMaterial();
    vec3 f = vec3(0.24f);
    mat->SetProperty<vec3>("PBRProperties", "F0", f);
    mat->SetTexture("textures.albedo", albedo);
    mat->SetTexture("textures.metallic", metallic);
    mat->SetTexture("textures.roughness", roughness);
    mat->SetTexture("textures.normal", normal);
    mp->SetMesh(sphereMesh);
    mp->SetMaterial(mat);
    return sphere;
}

inline void AddBillboard(GameObject* obj, Texture* texture)
{
    static auto billboardShader = Shader::Create("Billboard").WithSphericalBillboarding().WithUnlitSurface().Build();
    static Mesh* quadMesh =  Mesh::GetQuad();
    static bool allocate = true;
    if(allocate)
    {
        billboardShader->AllocateBuffers(100);
        allocate = false;
    }
    auto mp = obj->AddComponent<MeshComponent>();
    Material* mat = billboardShader->CreateMaterial();
    mat->SetTexture("billboard.texture", texture);
    vec2 size = vec2(0.05f);
    mat->SetProperty("Billboardprops", "size", size);
    mp->SetMesh(quadMesh);
    mp->SetMaterial(mat);
}

inline GameObject* CreateUnlitQuad(vec3 position, Shader* shader)
{
    static Texture* icon = Utilities::ImportTexture(GetAbsoluteResourcesPath("\\icons\\lightbulb_outline.png"), GL_TEXTURE_2D);
    Mesh* quadMesh =  Mesh::GetQuad();

    GameObject* quad = scene.InstantiateGameObject();
    quad->Name = "Quad";
    quad->transform.position = position;

    auto mp = quad->AddComponent<MeshComponent>();
    Material* mat = shader->CreateMaterial();
    mat->SetTexture("billboard.texture", icon);
    mp->SetMesh(quadMesh);
    mp->SetMaterial(mat);
    return quad;
}
std::vector<float> hs;
inline vector<GameObject*> CreateIsland(vec3 position, Shader* shader)
{
    static Texture* albedo =   Utilities::ImportTexture(GetAbsoluteResourcesPath("\\PBR_materials\\Ground\\Ground_Albedo.jpg"), GL_TEXTURE_2D);
    static Texture* metallic = Utilities::ImportTexture(GetAbsoluteResourcesPath("\\PBR_materials\\[4K]Tiles58\\Tiles58_met.jpg"), GL_TEXTURE_2D);
    static Texture* roughness =Utilities::ImportTexture(GetAbsoluteResourcesPath("\\PBR_materials\\Ground\\Ground_Roughness.jpg"), GL_TEXTURE_2D);
    static Texture* normal =   Utilities::ImportTexture(GetAbsoluteResourcesPath("\\PBR_materials\\Ground\\Ground_Normal.jpg"), GL_TEXTURE_2D);
    static Texture* heightmap = Utilities::ImportTexture(GetAbsoluteResourcesPath("\\heightmaps\\island_resized_128.png"));
    auto& segments = Mesh::FromHeightmap(
        heightmap,
        5,
        100,
        50, 
        2000);
    
    int i = 0;
    GameObject* island = scene.InstantiateGameObject();
    island->Name = "Island";
    vector<GameObject*> objs = { island };
    ivec2 max;
    for(auto seg : segments)
    {
        GameObject* segment = scene.InstantiateGameObject();
        auto segmentMesh = seg.first;
        auto segmentBounds = seg.second;
        max = Utilities::Max(max, segmentBounds.second);
        segment->Name = "Island segment " + Utilities::ToString(i);
        auto v = dynamic_cast<AxisAlignedBox*>(segmentMesh->GetBoundingVolume());
        auto pos = v->Min + (v->Max - v->Min) * 0.5f;
        // segment->transform.position = position + pos;
        auto mp = segment->AddComponent<MeshComponent>();
        // mp->SetMeshOffset(-pos);
        // mp->DrawBoundingBox = true;
        mp->SetMesh(segmentMesh);
        Material* mat = shader->CreateMaterial();
        vec3 f = vec3(0.24f);
        mat->SetProperty<vec3>("PBRProperties", "F0", f);
        mat->SetTexture("textures.albedo", albedo);
        mat->SetTexture("textures.metallic", metallic);
        mat->SetTexture("textures.roughness", roughness);
        mat->SetTexture("textures.normal", normal);
        mp->SetMaterial(mat);

        if(hs.size() == 0) for(Vertex& v : segmentMesh->GetVertices()) hs.push_back(v.Position.y);
        objs.push_back(segment);
        segment->transform.SetParent(&island->transform);
        i++;
    }
    auto pc = island->AddComponent<RigidBodyComponent>();
    Engine::Physics::ColliderInfo colInfo;
    colInfo.Transform = island->transform;
    colInfo.LocalScaling = {5, 1, 5};
    colInfo.Type = Engine::Physics::ColliderType::TERRAIN;
    colInfo.Terrain.Columns = heightmap->GetWidth();
    colInfo.Terrain.Rows = heightmap->GetHeight();
    colInfo.Terrain.Data = hs.data();
    colInfo.Terrain.HeightScale = 0.1f;
    colInfo.Terrain.MinHeight = -1000;
    colInfo.Terrain.MaxHeight = 1000;
    pc->Initialize(colInfo, 0.0f);
    pc->GetRigidBody().SetLinearConstraints({true,true,true});
    pc->GetRigidBody().SetKinematic(true);
    
    island->transform.SetGlobalPosition({-250, -70, -250});
    return objs;
}

inline GameObject* CreateSkybox(Shader* shader, Material* mat)
{
    static Texture* back = Utilities::ImportTexture(GetAbsoluteResourcesPath("\\texture\\skybox\\back.tga"));
    static Texture* front = Utilities::ImportTexture(GetAbsoluteResourcesPath("\\texture\\skybox\\front.tga"));
    static Texture* left = Utilities::ImportTexture(GetAbsoluteResourcesPath("\\texture\\skybox\\left.tga"));
    static Texture* right = Utilities::ImportTexture(GetAbsoluteResourcesPath("\\texture\\skybox\\right.tga"));
    static Texture* top = Utilities::ImportTexture(GetAbsoluteResourcesPath("\\texture\\skybox\\top.tga"));
    static Texture* bot = Utilities::ImportTexture(GetAbsoluteResourcesPath("\\texture\\sand_bot2.jpg"));
    static Texture* skyboxTexture = new Texture(right, left, top, bot, back, front);  
    static Mesh* cubeMesh =  Mesh::GetSkybox();

    GameObject* skybox = new GameObject();
    skybox->Name = "Skybox";
    auto mp = skybox->AddComponent<MeshComponent>();
    mp->SetMesh(cubeMesh);
    mat->SetTexture("skybox.texture", skyboxTexture);
    mp->SetMaterial(mat);
    return skybox;
}

inline GameObject* CreatePointLight(vec3 position, vec4 colour, float radius)
{
    GameObject* pointlight = scene.InstantiateGameObject();
    pointlight->Name = "Point Light";
    pointlight->transform.position = position;
    auto plight = pointlight->AddComponent<LightComponent>();
    plight->SetType(LightType::POINT);
    plight->PointLight().Colour = colour;
    plight->PointLight().Radius = radius;

    //Add light icon
    // AddBillboard(pointlight, lightbulbIcon);
    return pointlight;
}

inline GameObject* CreateDirectionalLight(vec4 colour)
{
    GameObject* light = scene.InstantiateGameObject();
    light->Name = "Directional Light";
    light->transform.position.y = 5.0f;
    light->transform.rotation = Quaternion::FromEuler({125.0f, 0.0f, 0.0f});
    auto lcomp = light->AddComponent<LightComponent>();
    lcomp->SetType(LightType::DIRECTIONAL);
    lcomp->SetColour(colour);
    // AddBillboard(light, lightbulbIcon);
    return light;
}

inline void DrawBB(AxisAlignedBox* box, vec4 color = vec4(1,1,0,0))
{
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
    ls.Colour = color;
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

inline void DrawViewFrustum(mat4 VP)
{
    vec4 v[8];
    v[0] = vec4(-1,-1,-1,1);
    v[1] = vec4(1,-1,-1,1);
    v[2] = vec4(1,1,-1,1);
    v[3] = vec4(-1,1,-1,1);

    v[4] = vec4(-1,-1,1,1);
    v[5] = vec4(1,-1,1,1);
    v[6] = vec4(1,1,1,1);
    v[7] = vec4(-1,1,1,1);
    for(int i = 0; i < 8; i++) v[i] = inverse(VP) * v[i];
    for(int i = 0; i < 8; i++) v[i] = v[i] * (1.0f / v[i].w);

    LineSegment ls;
    ls.Colour = {0, 1, 0, 0};
    ls.Vertices = {v[0], v[1], v[2], v[3], v[0]};
    Renderer::GetInstance()->DrawLineSegment(ls);
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

inline void DrawOctree(Octree::Octan* oct)
{
    DrawBB(oct->BoundingBox);
    for(int i = 0; i < 8; i++)
    {
        if(oct->octans[i] != nullptr) DrawOctree(oct->octans[i]);
    }
}