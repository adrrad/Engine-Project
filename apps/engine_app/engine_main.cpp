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
#include "assets/resources/MeshAsset.hpp"
#include "assets/resources/ImageAsset.hpp"
#include "assets/importing/MeshImporter.hpp"

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

GameObject* CreateSphere(vec3 position, Shader* shader)
{
    static Assets::ImageAsset* albedo =   Assets::AssetManager::GetInstance()->GetAsset<Assets::ImageAsset>("PBR_materials/[4K]Tiles58/Tiles58_col.jpg");
    static Assets::ImageAsset* metallic = Assets::AssetManager::GetInstance()->GetAsset<Assets::ImageAsset>("PBR_materials/[4K]Tiles58/Tiles58_met.jpg");
    static Assets::ImageAsset* roughness =Assets::AssetManager::GetInstance()->GetAsset<Assets::ImageAsset>("PBR_materials/[4K]Tiles58/Tiles58_rgh.jpg");
    static Assets::ImageAsset* normal =   Assets::AssetManager::GetInstance()->GetAsset<Assets::ImageAsset>("PBR_materials/[4K]Tiles58/Tiles58_nrm.jpg");
    static Assets::MeshAsset* spehereMesh = Assets::AssetManager::GetInstance()->GetAsset<Assets::MeshAsset>("models/sphere.obj");
    
    
    GameObject* sphere = scene.InstantiateGameObject();
    sphere->Name = "Sphere";
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

GameObject* CreateCube(vec3 position, Shader* shader)
{
    static Assets::ImageAsset* albedo =   Assets::AssetManager::GetInstance()->GetAsset<Assets::ImageAsset>("PBR_materials/[4K]Tiles58/Tiles58_col.jpg");
    static Assets::ImageAsset* metallic = Assets::AssetManager::GetInstance()->GetAsset<Assets::ImageAsset>("PBR_materials/[4K]Tiles58/Tiles58_met.jpg");
    static Assets::ImageAsset* roughness =Assets::AssetManager::GetInstance()->GetAsset<Assets::ImageAsset>("PBR_materials/[4K]Tiles58/Tiles58_rgh.jpg");
    static Assets::ImageAsset* normal =   Assets::AssetManager::GetInstance()->GetAsset<Assets::ImageAsset>("PBR_materials/[4K]Tiles58/Tiles58_nrm.jpg");
    static Assets::MeshAsset* cubeAsset = Assets::AssetManager::GetInstance()->GetAsset<Assets::MeshAsset>("models/cube2.obj");
    GameObject* sphere = scene.InstantiateGameObject();
    sphere->Name = "Sphere";
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
        new Texture(heightmap->GetImageData()),
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
    return objs;
}

GameObject* CreateSkybox(Shader* shader, Material* mat)
{
    static Assets::ImageAsset* back = Assets::AssetManager::GetInstance()->GetAsset<Assets::ImageAsset>("texture/skybox/back.tga");
    static Assets::ImageAsset* front = Assets::AssetManager::GetInstance()->GetAsset<Assets::ImageAsset>("texture/skybox/front.tga");
    static Assets::ImageAsset* left = Assets::AssetManager::GetInstance()->GetAsset<Assets::ImageAsset>("texture/skybox/left.tga");
    static Assets::ImageAsset* right = Assets::AssetManager::GetInstance()->GetAsset<Assets::ImageAsset>("texture/skybox/right.tga");
    static Assets::ImageAsset* top = Assets::AssetManager::GetInstance()->GetAsset<Assets::ImageAsset>("texture/skybox/top.tga");
    static Assets::ImageAsset* bot = Assets::AssetManager::GetInstance()->GetAsset<Assets::ImageAsset>("texture/sand_bot2.jpg");
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

GameObject* CreatePointLight(vec3 position, vec4 colour, float radius)
{
    GameObject* pointlight = scene.InstantiateGameObject();
    pointlight->Name = "Point Light";
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
    Platform::IO::Directory dir(Platform::IO::Path(std::string(RESOURCES_DIR)), true);
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
    
    ComponentManager::RegisterComponentPool<CameraComponent>();
    ComponentManager::RegisterComponentPool<InspectorCameraComponent>();
    ComponentManager::RegisterComponentPool<MeshComponent>();
    ComponentManager::RegisterComponentPool<RigidBodyComponent>();
    ComponentManager::RegisterComponentPool<LightComponent>();
    ComponentManager::RegisterComponentPool<MovementComponent>();
    // Platform::IO::File file = Platform::IO::File(RESOURCES_DIR+string("JSONTEST.json"));
    // string s = string(file.ReadAll().Data(), file.Size);
    // auto& scene_info = *JSON::ParseJSON(s);
    // Scene scenetest;
    // Utilities::Serialisation::DeserialiseObject<Scene>(&scenetest, scene_info);
    physicsManager->SetDebugDraw(true);
    editor.SetCurrentScene(&scene);

    GameObject* cameraObject_test = scene.InstantiateGameObject();
    cameraObject_test->Name = "TEST CAMERA";
    cameraObject_test->transform.position = glm::vec3(0, 0, 5);
    auto cam_test = cameraObject_test->AddComponent<CameraComponent>();
    cam_test->FarPlane = 100.0f;
    //scene.AddGameObject(cameraObject_test);

    GameObject* cameraObject = scene.InstantiateGameObject();
    cameraObject->Name = "Camera";
    cameraObject->transform.position = glm::vec3(0, 5, 10);
    auto cam = cameraObject->AddComponent<CameraComponent>();


    Shader* deferred = Shader::Create("Deferred").WithWorldSpaceVertexFunctions().WithGBuffer().Build();


    deferred->AllocateBuffers(5000);
    auto sphere1 = CreateSphere({0,20,0}, deferred );
    
    sphere1->transform.rotation = {0, 0, 0, 1};
    auto sphere2 = CreateCube({0,0,0}, deferred );
    auto sphere3 = CreateSphere({3,0,0}, deferred );
    sphere3->transform.rotation = glm::quat({0, 0, 90});
    // sphere0->Name = "Sphere 0";
    sphere1->Name = "Sphere 1";
    sphere2->Name = "Sphere 2";
    sphere3->Name = "Sphere 3";    
    std::vector<MeshComponent*> mps;
    int dim = 0;
    float posScale = 10.0f;
    int half = dim/2;
    auto spheres = scene.InstantiateGameObject();
    spheres->Name = "Spheres";
    //scene.AddGameObject(spheres);

    for(int x = -half; x < half; x++)
    {
        for(int y = -half; y < half; y++)
        {
            auto sphere = CreateSphere({x*posScale+10,0.0f,y*posScale}, deferred );
            sphere->Name = "Sphere " + std::to_string(x*dim+y);
            //scene.AddGameObject(sphere);
            mps.push_back(sphere->GetComponent<MeshComponent>());
            sphere->transform.SetParent(&spheres->transform);
            // sphere->GetComponent<MeshComponent>()->DrawBoundingBox = true;
        }        
    }

    auto islandSegments = CreateIsland(vec3(0, -95, 0), deferred );
    // watah->transform.rotation.x = 90.0f;
    // watah->transform.scale = vec3(1000, 1000, 1);
    //SKYBOX
    Shader* skyShader = Shader::Create("Skybox").WithSkyboxVertexFunctions().WithSkybox(true).Build();
    skyShader->AllocateBuffers(1);
    Material* skyMat = skyShader->CreateMaterial();
    auto skybox = CreateSkybox(skyShader, skyMat);
    
    //POINT LIGHTS
    auto p2 = CreatePointLight({-5,0,5}, {1.0f, 0.0f, 0.0f, 1.0f}, 50.0f);
    p2->Name = "Red Light";
    auto p3 = CreatePointLight({0,0,7}, {0.0f, 1.0f, 0.0f, 1.0f}, 50.0f);
    p3->Name = "Green Light";
    auto p = CreatePointLight({5,0,5}, {0.0f, 0.0f, 1.0f, 1.0f}, 50.0f);
    p->Name = "Blue Light";
    auto d = CreateDirectionalLight(vec4(1));
    d->GetComponent<LightComponent>()->SetDebugDrawDirectionEnabled(true);

    // scene.AddGameObject(sphere0);
    //scene.AddGameObject(sphere1);
    //scene.AddGameObject(sphere2);
    //scene.AddGameObject(sphere3);

    Engine::Physics::ColliderInfo colInfo;
    colInfo.Transform = sphere1->transform;
    colInfo.Type = Engine::Physics::ColliderType::SPHERE;
    colInfo.Sphere.Radius = 1.0f;
    // colInfo.LocalScaling = {10, 10, 10};
    // RIGIDBODIES
    auto rbc = sphere1->AddComponent<RigidBodyComponent>();
    rbc->Initialize(colInfo, 1);

    colInfo.Transform = sphere2->transform;
    colInfo.Type = Engine::Physics::ColliderType::BOX;
    AxisAlignedBox* b = ((AxisAlignedBox*)sphere2->GetComponent<MeshComponent>()->GetBoundingVolume());
    colInfo.Box.HalfExtents = (b->Max - b->Min) * 0.5f;
    auto rbc2 = sphere2->AddComponent<RigidBodyComponent>();
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

    std::vector<Octree::GOBB> gos;
    for(auto go : scene.GetGameObjects())
    {
        auto mp = go->GetComponent<MeshComponent>();
        if(mp) gos.push_back({go, (AxisAlignedBox*)mp->GetBoundingVolume()});
    }
    Engine::Acceleration::Octree* tree = new Octree(gos, 4);

    std::vector<Octree::GOBB> segments;
    for(auto go : islandSegments)
    {
        auto mp = go->GetComponent<MeshComponent>();
        if(mp) segments.push_back({go, (AxisAlignedBox*)mp->GetBoundingVolume()});
    }
    Engine::Acceleration::Octree* tree_island = new Octree(segments, 4);

    ivec2 winDims = Renderer::GetInstance()->GetWindowDimensions();
    auto gBuffer = Framebuffer::Create(winDims.x, winDims.y)
                    .WithColorbuffer("position", GL_RGBA16F)
                    .WithColorbuffer("normal", GL_RGBA16F)
                    .WithColorbuffer("reflectance", GL_RGBA16F)
                    .WithColorbuffer("albedospec", GL_RGBA)
                    .WithColorbuffer("depth", GL_R16)
                    .WithDepthbuffer("depth")
                    .Build();

    auto lightBuffer = Framebuffer::Create(winDims.x, winDims.y)
                        .WithColorbuffer("colour", GL_RGBA)
                        .Build();

    Shader* light = Shader::Create("Light").WithPPVertexFunctions().WithDeferredPBRLighting().Build();
    light->AllocateBuffers(10);
    Material* mat = light->CreateMaterial();
    mat->SetTexture("gBuffer.position", gBuffer->GetColorbuffer("position"));
    mat->SetTexture("gBuffer.normal", gBuffer->GetColorbuffer("normal"));
    mat->SetTexture("gBuffer.reflectance", gBuffer->GetColorbuffer("reflectance"));
    mat->SetTexture("gBuffer.albedoSpec", gBuffer->GetColorbuffer("albedospec"));
    mat->SetTexture("gBuffer.depth", gBuffer->GetColorbuffer("depth"));
    skyMat->SetTexture("gBuffer.depth", gBuffer->GetColorbuffer("depth"));
    skyMat->SetTexture("lBuffer.colour", lightBuffer->GetColorbuffer("colour"));
    auto quad = Mesh::GetQuad();
    auto postprocessingQuad = new GameObject();// scene.InstantiateGameObject();
    postprocessingQuad->Name = "PostProcessingQuad";
    //scene.AddGameObject(postprocessingQuad);
    auto ppmp = postprocessingQuad->AddComponent<MeshComponent>();
    ppmp->SetMesh(quad);
    ppmp->SetMaterial(mat);

    auto createRenderpass = [&, tree, tree_island](){
        auto rpb = Renderpass::Create()
            .NewSubpass("Geometry", SubpassFlags::DEFAULT, 50000)
            .UseFramebuffer(gBuffer);
            // .DrawMesh(island->GetComponent<MeshComponent>())
            // .DrawMesh(watah->GetComponent<MeshComponent>());
        Frustum& frustum = CameraComponent::GetMainCamera()->GetViewFrustum();
        // for(auto seg : islandSegments) rpb.DrawMesh(seg->GetComponent<MeshComponent>());
        tree_island->RecordRenderpass(&frustum, rpb);
        tree->Rebuild();
        tree->RecordRenderpass(&frustum, rpb);
        rpb.NewSubpass("Lighting")
            .UseFramebuffer(lightBuffer)
            .DrawMesh(ppmp)
            .NewSubpass("Skybox", SubpassFlags::DISABLE_DEPTHMASK)
            .UseFramebuffer(Framebuffer::GetDefault())
            .DrawMesh(skybox->GetComponent<MeshComponent>())
            .NewSubpass("Overlay", SubpassFlags::DISABLE_DEPTHMASK | SubpassFlags::ENABLE_BLENDING);
        return rpb.Build();
    };

    auto call = [&](float dt)
    {
        physicsManager->Update(dt);
        renderer->InvalidateRenderpass();
        renderer->SetRenderpass(createRenderpass());
    };

    Platform::WindowManager::GetInstance()->RegisterWindowResizeCallback([&](int w, int h){
        winDims = Renderer::GetInstance()->GetWindowDimensions();
        lightBuffer->Rebuild(w,h);
        gBuffer->Rebuild(w, h);
    });

    renderer->SetRenderpassReconstructionCallback(createRenderpass);
    renderer->SetScene(&scene);
    auto wm = Platform::WindowManager::GetInstance();
    wm->MaximizeWindow(wm->GetActiveWindow());

    auto json = Engine::Utilities::Serialisation::SerialiseObject(&scene)->ToString();
    auto chars = Utilities::GetCharPtr(json);
    auto fname = RESOURCES_DIR+std::string("/JSONTEST.json");
    Engine::Platform::IO::File f(fname);
    f.Open(Platform::IO::File::TRUNCATE | Platform::IO::File::WRITE);
    f.Write(chars);
    auto compman = ComponentManager::GetInstance()->GetComponentPools();
    editor.Run();
    return 0;
}

int main()
{
    scene2();
}