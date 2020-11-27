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

#include "gamecomponents/StatsComponent.hpp"

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
#include "utilities/ImageProcessing.hpp"

#include "core/EngineSubsystem.hpp"
#include "core/EngineCore.hpp"

#include "platform/io/File.hpp"
#include "platform/io/Directory.hpp"
#include "platform/io/Filesystem.hpp"

#include "utilities/json/JSON.hpp"
#include "assets/Asset.hpp"
#include "assets/AssetManager.hpp"
#include "assets/resources/MeshAsset.hpp"
#include "assets/resources/ImageAsset.hpp"
#include "assets/importing/MeshImporter.hpp"
#include "assets/resources/JSONAsset.hpp"



#include <glm/glm.hpp>

using namespace std;
using namespace glm;
using namespace Engine;
using namespace Engine::Rendering;
using namespace Engine::Components;
using namespace Engine::Utilities;
using namespace Engine::Core;
using namespace Engine::Geometry;
using namespace Engine::Acceleration;

Scene scene = Scene();

GameObject* CreateSphere(vec3 position, Shader* shader, std::string name = "Sphere")
{
    static Assets::ImageAsset* albedo =   Assets::AssetManager::GetInstance()->GetAsset<Assets::ImageAsset>("PBR_materials/[4K]Tiles58/Tiles58_col.jpg");
    static Assets::ImageAsset* metallic = Assets::AssetManager::GetInstance()->GetAsset<Assets::ImageAsset>("PBR_materials/[4K]Tiles58/Tiles58_met.jpg");
    static Assets::ImageAsset* roughness =Assets::AssetManager::GetInstance()->GetAsset<Assets::ImageAsset>("PBR_materials/[4K]Tiles58/Tiles58_rgh.jpg");
    static Assets::ImageAsset* normal =   Assets::AssetManager::GetInstance()->GetAsset<Assets::ImageAsset>("PBR_materials/[4K]Tiles58/Tiles58_nrm.jpg");
    static Assets::MeshAsset* spehereMesh = Assets::AssetManager::GetInstance()->GetAsset<Assets::MeshAsset>("models/sphere.obj");
    
    
    GameObject* sphere = scene.InstantiateGameObject();
    sphere->Name = name;
    sphere->transform.position = position;

    auto mp = sphere->AddComponent<MeshComponent>();
    Material* mat = shader->CreateMaterial();
    vec3 f = vec3(0.24f);
    mat->SetProperty<vec3>("PBRProperties", "F0", f);
    mat->UseTextureAsset("textures.albedo", albedo);
    mat->UseTextureAsset("textures.metallic", metallic);
    mat->UseTextureAsset("textures.roughness", roughness);
    mat->UseTextureAsset("textures.normal", normal);
    mp->UseMeshAsset(spehereMesh);
    mp->SetMaterial(mat);
    return sphere;
}

GameObject* CreateCube(vec3 position, Shader* shader, std::string name = "Cube")
{
    static Assets::ImageAsset* albedo =   Assets::AssetManager::GetInstance()->GetAsset<Assets::ImageAsset>("PBR_materials/[4K]Planks16/Planks16_col.jpg");
    static Assets::ImageAsset* metallic = Assets::AssetManager::GetInstance()->GetAsset<Assets::ImageAsset>("PBR_materials/[4K]Planks16/Planks16_met.jpg");
    static Assets::ImageAsset* roughness =Assets::AssetManager::GetInstance()->GetAsset<Assets::ImageAsset>("PBR_materials/[4K]Planks16/Planks16_rgh.jpg");
    static Assets::ImageAsset* normal =   Assets::AssetManager::GetInstance()->GetAsset<Assets::ImageAsset>("PBR_materials/[4K]Planks16/Planks16_nrm.jpg");
    static Assets::MeshAsset* cubeAsset = Assets::AssetManager::GetInstance()->GetAsset<Assets::MeshAsset>("models/cube2.obj");
    GameObject* sphere = scene.InstantiateGameObject();
    sphere->Name = name;
    sphere->transform.position = position;

    auto mp = sphere->AddComponent<MeshComponent>();
    Material* mat = shader->CreateMaterial();
    vec3 f = vec3(0.24f);
    mat->SetProperty<vec3>("PBRProperties", "F0", f);
    mat->UseTextureAsset("textures.albedo", albedo);
    mat->UseTextureAsset("textures.metallic", metallic);
    mat->UseTextureAsset("textures.roughness", roughness);
    mat->UseTextureAsset("textures.normal", normal);
    mp->UseMeshAsset(cubeAsset);
    mp->SetMaterial(mat);
    return sphere;
}

std::vector<float> hs;
vector<GameObject*> CreateIsland(vec3 position, Shader* shader)
{
    static Assets::ImageAsset* albedo =   Assets::AssetManager::GetInstance()->GetAsset<Assets::ImageAsset>("PBR_materials/Ground/Ground_Albedo.jpg");
    static Assets::ImageAsset* metallic = Assets::AssetManager::GetInstance()->GetAsset<Assets::ImageAsset>("PBR_materials/[4K]Tiles58/Tiles58_met.jpg");
    static Assets::ImageAsset* roughness =Assets::AssetManager::GetInstance()->GetAsset<Assets::ImageAsset>("PBR_materials/Ground/Ground_Roughness.jpg");
    static Assets::ImageAsset* normal =   Assets::AssetManager::GetInstance()->GetAsset<Assets::ImageAsset>("PBR_materials/Ground/Ground_Normal.jpg");
    static Assets::ImageAsset* heightmap = Assets::AssetManager::GetInstance()->GetAsset<Assets::ImageAsset>("heightmaps/island_resized_128.png");
    auto& segments = Mesh::FromHeightmap(
        heightmap->GetImageData(),
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
        mat->UseTextureAsset("textures.albedo", albedo);
        mat->UseTextureAsset("textures.metallic", metallic);
        mat->UseTextureAsset("textures.roughness", roughness);
        mat->UseTextureAsset("textures.normal", normal);
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
    colInfo.Terrain.Columns = heightmap->GetImageData()->Width;
    colInfo.Terrain.Rows = heightmap->GetImageData()->Height;
    colInfo.Terrain.Data = hs.data();
    colInfo.Terrain.HeightScale = 0.1f;
    colInfo.Terrain.MinHeight = -1000;
    colInfo.Terrain.MaxHeight = 1000;
    pc->Initialize(colInfo, 0.0f);
    pc->GetRigidBody().SetLinearConstraints({true,true,true});
    pc->GetRigidBody().SetKinematic(true);
    
    island->transform.SetGlobalPosition({-250, -70, -250});
    island->SetStatic(true);
    return objs;
}

GameObject* CreatePointLight(vec3 position, vec4 colour, float radius, std::string name = "Point Light")
{
    GameObject* pointlight = scene.InstantiateGameObject();
    pointlight->Name = name;
    pointlight->transform.position = position;
    auto plight = pointlight->AddComponent<LightComponent>();
    plight->SetType(LightType::POINT);
    plight->PointLight().Colour = colour;
    plight->PointLight().Radius = radius;
    return pointlight;
}

GameObject* CreateDirectionalLight(vec4 colour)
{
    GameObject* light = scene.InstantiateGameObject();
    light->Name = "Directional Light";
    light->transform.position.y = 5.0f;
    light->transform.rotation = Quaternion::FromEuler({125.0f, 0.0f, 0.0f});
    auto lcomp = light->AddComponent<LightComponent>();
    lcomp->SetType(LightType::DIRECTIONAL);
    lcomp->SetColour(colour);
    return light;
}

void DrawBB(AxisAlignedBox* box, vec4 color = vec4(1,1,0,0))
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

void DrawViewFrustum(mat4 VP)
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

void DrawOctree(Octree::Octan* oct)
{
    DrawBB(oct->BoundingBox);
    for(int i = 0; i < 8; i++)
    {
        if(oct->octans[i] != nullptr) DrawOctree(oct->octans[i]);
    }
}

int scene2()
{
    Platform::IO::Directory dir(Platform::IO::Path(std::string(RESOURCES_DIR)));
    EngineSettings settings;
    settings.Project.ProjectName = "Engine";
    settings.Project.RootDirectory = RESOURCES_DIR;
    settings.Window.Width = 1600;
    settings.Window.Height = 1024;
    settings.Window.Maximized = false;
    Engine::Editor::EditorCore editor(settings);
    Engine::Assets::AssetManager* manager = Assets::AssetManager::GetInstance();
    // manager->SaveAssetDatabase();
    Renderer* renderer = Renderer::GetInstance();
    Engine::Physics::PhysicsManager* physicsManager = Engine::Physics::PhysicsManager::GetInstance();
    physicsManager->SetDebugDraw(true);
    editor.SetCurrentScene(&scene);
    Assets::JSONAsset* sceneJSON = Assets::AssetManager::GetInstance()->GetAsset<Assets::JSONAsset>("first_scen1e.json");
    if(sceneJSON != nullptr)
    {
        sceneJSON->Load();
        scene.Deserialise(sceneJSON->GetJSONObject());
        sceneJSON->Free();
    }
    else
    {

        GameObject* cameraObject = scene.InstantiateGameObject();
        cameraObject->Name = "Camera";
        cameraObject->transform.position = glm::vec3(0, -10, 10);
        auto cam = cameraObject->AddComponent<CameraComponent>();


        Shader* deferred = Renderer::GetInstance()->GetShader("Deferred");

        auto segments = CreateIsland({0,0,0}, deferred);

        auto sphere1 = CreateSphere({0,20,0}, deferred, "Sphere 1");
        auto cube = CreateCube({0,0,0}, deferred, "Cube");
        auto sphere3 = CreateSphere({3,0,0}, deferred, "Sphere 2");


        int dim = 0;
        float posScale = 10.0f;
        int half = dim/2;
        auto spheres = scene.InstantiateGameObject();
        spheres->Name = "Spheres";

        for(int x = -half; x < half; x++)
        {
            for(int y = -half; y < half; y++)
            {
                auto sphere = CreateSphere({x*posScale+10,5.0f,y*posScale}, deferred );
                sphere->Name = "Sphere " + std::to_string(x*dim+y);
                // sphere->transform.SetParent(&spheres->transform);
                // sphere->GetComponent<MeshComponent>()->DrawBoundingBox = true;
                Engine::Physics::ColliderInfo colInfo;
                colInfo.Transform = sphere->transform;
                colInfo.Type = Engine::Physics::ColliderType::SPHERE;
                colInfo.Sphere.Radius = 1.0f;

                auto rbc = sphere->AddComponent<RigidBodyComponent>();
                rbc->Initialize(colInfo, 1);

            }        
        }
        
        //POINT LIGHTS
        auto p2 = CreatePointLight({-5,0,5}, {1.0f, 0.0f, 0.0f, 1.0f}, 50.0f, "Red Light");
        auto p3 = CreatePointLight({0,0,7}, {0.0f, 1.0f, 0.0f, 1.0f}, 50.0f, "Green Light");
        auto p = CreatePointLight({5,0,5}, {0.0f, 0.0f, 1.0f, 1.0f}, 50.0f, "Blue Light");
        auto d = CreateDirectionalLight(vec4(1));

        // ------------------------- RIGIDBODIES ------------------------- 
        Engine::Physics::ColliderInfo colInfo;
        colInfo.Transform = sphere1->transform;
        colInfo.Type = Engine::Physics::ColliderType::SPHERE;
        colInfo.Sphere.Radius = 1.0f;

        auto rbc = sphere1->AddComponent<RigidBodyComponent>();
        rbc->Initialize(colInfo, 1);

        colInfo.Transform = cube->transform;
        colInfo.Type = Engine::Physics::ColliderType::BOX;
        AxisAlignedBox* b = ((AxisAlignedBox*)cube->GetComponent<MeshComponent>()->GetBoundingVolume());
        colInfo.Box.HalfExtents = (b->Max - b->Min) * 0.5f;
        auto rbc2 = cube->AddComponent<RigidBodyComponent>();
        rbc2->Initialize(colInfo, 1);
        rbc2->GetRigidBody().SetKinematic(true);

        auto rbc3 = sphere3->AddComponent<RigidBodyComponent>();
        colInfo.Transform = sphere3->transform;
        colInfo.Type = Engine::Physics::ColliderType::SPHERE;
        colInfo.Sphere.Radius = 1.0f;
        rbc3->Initialize(colInfo, 1);

        auto camrbc = cameraObject->AddComponent<RigidBodyComponent>();
        colInfo.Transform = cameraObject->transform;
        colInfo.Type = Engine::Physics::ColliderType::SPHERE;
        colInfo.Sphere.Radius = 1.0f;
        camrbc->Initialize(colInfo, 100.0f);
        camrbc->GetRigidBody().SetAngularConstraints({true, true, true});
        camrbc->GetRigidBody().SetLinearConstraints({true, false, true});
        auto movement = cameraObject->AddComponent<MovementComponent>();
        movement->SetCamera(cam);

        // auto json = scene.Serialise()->ToString();
        // Assets::JSONAsset* sceneJSON = Assets::AssetManager::GetInstance()->CreateAsset<Assets::JSONAsset>("first_scene.json");
        // sceneJSON->ResourceFile->Open(Platform::IO::File::WRITE | Platform::IO::File::TRUNCATE);
        // sceneJSON->ResourceFile->Write(json);
        // sceneJSON->ResourceFile->Close();
    }




    auto wm = Platform::WindowManager::GetInstance();
    wm->MaximizeWindow(wm->GetActiveWindow());

    editor.Run();
    return 0;
}

int main()
{
    scene2();
}