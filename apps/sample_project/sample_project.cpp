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
#include "components/RigidBodyComponent.hpp"

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

#include "utils.hpp"

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

    editor.SetCurrentScene(&scene);


    GameObject* cameraObject = scene.InstantiateGameObject();
    cameraObject->Name = "Camera";
    cameraObject->transform.position = glm::vec3(0, 5, 10);
    auto cam = cameraObject->AddComponent<CameraComponent>();


    Shader* deferred = Shader::Create("Deferred").WithWorldSpaceVertexFunctions().WithGBuffer().Build();


    deferred->AllocateBuffers(5000);

    //SKYBOX
    Shader* skyShader = Shader::Create("Skybox").WithSkyboxVertexFunctions().WithSkybox(true).Build();
    skyShader->AllocateBuffers(1);
    Material* skyMat = skyShader->CreateMaterial();
    auto skybox = CreateSkybox(skyShader, skyMat);


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
        Frustum& frustum = CameraComponent::GetMainCamera()->GetViewFrustum();
        auto rpb = Renderpass::Create()
            .NewSubpass("Geometry", SubpassFlags::DEFAULT, 50000)
            .UseFramebuffer(gBuffer);
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