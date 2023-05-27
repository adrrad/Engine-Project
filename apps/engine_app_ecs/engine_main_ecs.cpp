#include "rendering/Renderer.hpp"
#include "rendering/Mesh.hpp"
#include "gameplay/Scene.hpp"
#include "gameplay/GameObject.hpp"
#include "core/EngineSettings.hpp"
#include "rendering/Renderpass.hpp"
#include "utilities/Utilities.hpp"

#include "components/CameraComponent.hpp"
#include "components/MovementComponent.hpp"
#include "components/LightComponent.hpp"
#include "components/MeshComponent.hpp"
#include "components/InspectorCameraComponent.hpp"

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
#include "assets/resources/BVHAsset.hpp"

#include "utilities/bvh/BVHAnimation.hpp"
#include "memory/PoolAllocator.hpp"

#include "rendering/HLRenderer.hpp"

#include "memory/MemoryManager.hpp"
#include "memory/SimpleAllocator.hpp"
#include "EngineComponent.hpp"
#include <glm/glm.hpp>



#include "gameplay/ecs/ECSManager.hpp"
#include "components/ECSComponents.hpp"

using namespace std;
using namespace glm;
using namespace Engine;
using namespace Rendering;
using namespace Gameplay;
using namespace Utilities;
using namespace Core;
using namespace Geometry;
using namespace Acceleration;

Scene* scene = nullptr;
ECSManager* mng = nullptr;

GameObject* CreateSphere(vec3 position, Shader* shader, std::string name = "Sphere")
{
    static Assets::ImageAsset* albedo =   Assets::AssetManager::GetInstance()->GetAsset<Assets::ImageAsset>("PBR_materials/[4K]Tiles58/Tiles58_col.jpg");
    static Assets::ImageAsset* metallic = Assets::AssetManager::GetInstance()->GetAsset<Assets::ImageAsset>("PBR_materials/[4K]Tiles58/Tiles58_met.jpg");
    static Assets::ImageAsset* roughness =Assets::AssetManager::GetInstance()->GetAsset<Assets::ImageAsset>("PBR_materials/[4K]Tiles58/Tiles58_rgh.jpg");
    static Assets::ImageAsset* normal =   Assets::AssetManager::GetInstance()->GetAsset<Assets::ImageAsset>("PBR_materials/[4K]Tiles58/Tiles58_nrm.jpg");
    static Assets::MeshAsset* spehereMesh = Assets::AssetManager::GetInstance()->GetAsset<Assets::MeshAsset>("models/sphere.obj");
    
    Renderer* r = Renderer::GetInstance();
    GameObject* sphere = scene->InstantiateGameObject();
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


    EntityID e = mng->GetEntity();

    mng->AddComponent<ECS::TransformComponent>(e);
    mng->AddComponent<ECS::MeshComponent>(e);
    mng->AddComponent<ECS::MaterialComponent>(e);

    auto& t = mng->GetComponent<ECS::TransformComponent>(e);
    auto& m = mng->GetComponent<ECS::MeshComponent>(e);
    auto& mt = mng->GetComponent<ECS::MaterialComponent>(e);
    auto mesh = r->GetMesh(spehereMesh->ID);
    t.position = position;
    t.rotation = Quaternion::FromEuler({0.0f, 0.0f, 0.0f});
    t.scale = {1.0f, 1.0f, 1.0f};

    m.EBO = mesh->GetEBO();
    m.VAO = mesh->GetVAO();
    m.VBO = mesh->GetVBO();
    m.IndexCount = mesh->GetIndexCount();
    mt.MaterialInstance = mat;
    mt.InstanceIndex = mat->GetInstanceIndex();
    mt.ShaderID = mat->GetShader()->GetProgramID();
    mt.VAO = m.VAO;
    mt.NumTextures = 4;

    mt.Textures[0].TextureID = mat->GetTexture("textures.albedo")->GetID();
    mt.Textures[0].UniformLocation = mat->GetShader()->GetUniformLocation("textures.albedo");
    
    mt.Textures[1].TextureID = mat->GetTexture("textures.metallic")->GetID();
    mt.Textures[1].UniformLocation = mat->GetShader()->GetUniformLocation("textures.metallic");
    
    mt.Textures[2].TextureID = mat->GetTexture("textures.roughness")->GetID();
    mt.Textures[2].UniformLocation = mat->GetShader()->GetUniformLocation("textures.roughness");
    
    mt.Textures[3].TextureID = mat->GetTexture("textures.normal")->GetID();
    mt.Textures[3].UniformLocation = mat->GetShader()->GetUniformLocation("textures.normal");
    

    return sphere;
}

GameObject* CreateQuad(vec3 position, Shader* shader, std::string name = "Quad")
{
    static Assets::ImageAsset* albedo =   Assets::AssetManager::GetInstance()->GetAsset<Assets::ImageAsset>("PBR_materials/[4K]Tiles58/Tiles58_col.jpg");
    static Assets::ImageAsset* metallic = Assets::AssetManager::GetInstance()->GetAsset<Assets::ImageAsset>("PBR_materials/[4K]Tiles58/Tiles58_met.jpg");
    static Assets::ImageAsset* roughness =Assets::AssetManager::GetInstance()->GetAsset<Assets::ImageAsset>("PBR_materials/[4K]Tiles58/Tiles58_rgh.jpg");
    static Assets::ImageAsset* normal =   Assets::AssetManager::GetInstance()->GetAsset<Assets::ImageAsset>("PBR_materials/[4K]Tiles58/Tiles58_nrm.jpg");

    GameObject* quad = scene->InstantiateGameObject();
    quad->Name = name;
    quad->transform.position = position;

    auto mp = quad->AddComponent<MeshComponent>();
    Material* mat = shader->CreateMaterial();
    vec3 f = vec3(0.24f);
    mat->SetProperty<vec3>("PBRProperties", "F0", f);
    mat->UseTextureAsset("textures.albedo", albedo);
    mat->UseTextureAsset("textures.metallic", metallic);
    mat->UseTextureAsset("textures.roughness", roughness);
    mat->UseTextureAsset("textures.normal", normal);
    mp->SetMesh(Mesh::GetQuad());
    mp->SetMaterial(mat);
    return quad;
}

GameObject* CreatePointLight(vec3 position, vec4 colour, float radius, std::string name = "Point Light")
{
    GameObject* pointlight = scene->InstantiateGameObject();
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
    GameObject* light = scene->InstantiateGameObject();
    light->Name = "Directional Light";
    light->transform.position = {0.0f, 250.0f, 200.0f};
    light->transform.rotation = Quaternion::FromEuler({-45.0f, 0.0f, 0.0f});
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
    mng = ECSManager::GetInstance();
    Platform::IO::Directory dir(Platform::IO::Path(std::string(RESOURCES_DIR)));
    const EngineSettings& settings = EngineSettings::Get();

    Core::EngineCore editor;
    Assets::AssetManager* manager = Assets::AssetManager::GetInstance();
    scene = Gameplay::SceneManager::GetInstance()->CreateNewScene("test");

    Renderer* renderer = Renderer::GetInstance();
    Physics::PhysicsManager* physicsManager = Physics::PhysicsManager::GetInstance();
    physicsManager->SetDebugDraw(true);


    GameObject* cameraObject = scene->InstantiateGameObject();
    cameraObject->Name = "Camera";
    cameraObject->transform.position = glm::vec3(0, -10, 10);
    auto cam = cameraObject->AddComponent<CameraComponent>();


    Shader* deferred = Renderer::GetInstance()->GetShader("Deferred");

    // auto segments = CreateIsland({0,0,0}, deferred);

    auto sphere1 = CreateSphere({0,-10,0}, deferred, "Sphere 1");
    auto sphere3 = CreateSphere({3,0,0}, deferred, "Sphere 2");
    auto quad = CreateQuad({0,0,0}, deferred, "Quad");
    quad->transform.position = {0,-20,0};
    quad->transform.scale = {50,50,50};
    quad->transform.rotation = Quaternion::FromEuler({-90,0,0});

    int dim = 5;
    float posScale = 10.0f;
    int half = dim/2;
    auto spheres = scene->InstantiateGameObject();
    spheres->Name = "Spheres";

    for(int x = -half; x < half; x++)
    {
        for(int y = -half; y < half; y++)
        {
            auto sphere = CreateSphere({x*posScale+10,5.0f,y*posScale}, deferred );
            sphere->Name = "Sphere " + std::to_string(x*dim+y);
            Physics::ColliderInfo colInfo;
            colInfo.Transform = sphere->transform;
            colInfo.Type = Physics::ColliderType::SPHERE;
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

    auto movement = cameraObject->AddComponent<MovementComponent>();
    movement->SetCamera(cam);


    ECSManager* ecsManager = ECSManager::GetInstance();




    auto wm = Platform::WindowManager::GetInstance();
    wm->MaximizeWindow(wm->GetActiveWindow());
    editor.Start();
    editor.GameLoop();
    return 0;
}

int main()
{
    scene2();
}