#include "renderer/Renderer.hpp"
#include "renderer/Mesh.hpp"
#include "renderer/Scene.hpp"
#include "renderer/Renderpass.hpp"
#include "utilities/Utilities.hpp"

#include "components/CameraComponent.hpp"
#include "components/MovementComponent.hpp"
#include "components/LightComponent.hpp"
#include "components/MeshComponent.hpp"
#include "components/WaveManagerComponent.hpp"
#include "components/SkyboxComponent.hpp"
#include "SlerpComponent.hpp"

#include <iostream>


using namespace std;
using namespace glm;
using namespace Rendering;
using namespace Components;
using namespace Utilities;


SceneObject* CreateSphere(vec3 position, Shader* shader)
{
    static Texture* albedo =   Utilities::ImportTexture(GetAbsoluteResourcesPath("\\PBR_materials\\[4K]Tiles58\\Tiles58_col.jpg"), GL_TEXTURE_2D);
    static Texture* metallic = Utilities::ImportTexture(GetAbsoluteResourcesPath("\\PBR_materials\\[4K]Tiles58\\Tiles58_met.jpg"), GL_TEXTURE_2D);
    static Texture* roughness =Utilities::ImportTexture(GetAbsoluteResourcesPath("\\PBR_materials\\[4K]Tiles58\\Tiles58_rgh.jpg"), GL_TEXTURE_2D);
    static Texture* normal =   Utilities::ImportTexture(GetAbsoluteResourcesPath("\\PBR_materials\\[4K]Tiles58\\Tiles58_nrm.jpg"), GL_TEXTURE_2D);
    Mesh* sphereMesh =  Mesh::GetSphere(shader);

    SceneObject* sphere = new SceneObject();
    sphere->Name = "Sphere";
    sphere->transform.position = position;

    auto mp = sphere->AddComponent<MeshComponent>();
    mp->SetMesh(sphereMesh);
    Material* mat = shader->CreateMaterial();
    vec3 f = vec3(0.24);
    mat->SetProperty<vec3>("PBRProperties", "F0", f);
    mat->SetTexture("textures.albedo", albedo);
    mat->SetTexture("textures.metallic", metallic);
    mat->SetTexture("textures.roughness", roughness);
    mat->SetTexture("textures.normal", normal);
    mp->SetMaterial(mat);
    return sphere;
}

SceneObject* CreateSkybox()
{
    static Texture* back = Utilities::ImportTexture(GetAbsoluteResourcesPath("\\texture\\skybox\\back.tga"));
    static Texture* front = Utilities::ImportTexture(GetAbsoluteResourcesPath("\\texture\\skybox\\front.tga"));
    static Texture* left = Utilities::ImportTexture(GetAbsoluteResourcesPath("\\texture\\skybox\\left.tga"));
    static Texture* right = Utilities::ImportTexture(GetAbsoluteResourcesPath("\\texture\\skybox\\right.tga"));
    static Texture* top = Utilities::ImportTexture(GetAbsoluteResourcesPath("\\texture\\skybox\\top.tga"));
    static Texture* bot = Utilities::ImportTexture(GetAbsoluteResourcesPath("\\texture\\sand_bot2.jpg"));
    static Texture* skyboxTexture = new Texture(right, left, top, bot, back, front);  
    Shader* shader = Shader::Create("Skybox").WithSkyboxVertexFunctions().WithSkybox().Build();
    shader->AllocateBuffers(1);
    static Mesh* cubeMesh =  Mesh::GetSkybox(shader);

    SceneObject* skybox = new SceneObject();
    auto mp = skybox->AddComponent<MeshComponent>();
    auto sb = skybox ->AddComponent<SkyboxComponent>();
    mp->SetMesh(cubeMesh);
    Material* mat = shader->CreateMaterial();
    mat->SetTexture("skybox.texture", skyboxTexture);
    mp->SetMaterial(mat);
    return skybox;
}

SceneObject* CreatePointLight(vec3 position, vec4 colour, float radius)
{
    SceneObject* pointlight = new SceneObject();
    pointlight->Name = "Point Light";
    pointlight->transform.position = position;
    auto plight = pointlight->AddComponent<LightComponent>();
    plight->SetType(LightType::POINT);
    plight->PointLight().Colour = colour;
    plight->PointLight().Radius = radius;
    return pointlight;
}

SceneObject* CreateDirectionalLight(vec4 colour)
{
    SceneObject* light = new SceneObject();
    light->Name = "Directional Light";
    light->transform.position.y = 5.0f;
    light->transform.rotation = {-135.0f, 0.0f, 0.0f};
    auto lcomp = light->AddComponent<LightComponent>();
    lcomp->SetType(LightType::DIRECTIONAL);
    lcomp->SetColour(colour);
    return light;
}

int scene2(bool testDeferred)
{
    Renderer* renderer = Renderer::GetInstance();
    Scene scene = Scene();

    SceneObject* cameraObject = new SceneObject();
    cameraObject->Name = "Camera";
    cameraObject->transform.position = glm::vec3(0, 0, 5);
    auto cam = cameraObject->AddComponent<CameraComponent>();
    auto movement = cameraObject->AddComponent<MovementComponent>();
    movement->SetCamera(cam);


    Shader* shader = Shader::Create("PBR").WithStandardVertexFunctions().WithPBR().Build();
    Shader* deferred = Shader::Create("Deferred").WithWorldSpaceVertexFunctions().WithGBuffer().Build();

    shader->AllocateBuffers(10);
    deferred->AllocateBuffers(10);
    auto sphere1 = CreateSphere({-3,0,0}, testDeferred ? deferred : shader);
    sphere1->transform.rotation = {0, 90, 0};
    auto sphere3 = CreateSphere({3,0,0}, testDeferred ? deferred : shader);
    sphere3->transform.rotation = {0, 0, 90};
    auto sphere2 = CreateSphere({0,0,0}, testDeferred ? deferred : shader);

    auto slerp = sphere2->AddComponent<SlerpComponent>();
    slerp->SetTransforms(&sphere1->transform, &sphere3->transform);
    auto skybox = CreateSkybox();
    auto p2 = CreatePointLight({-5,0,5}, {1.0f, 0.0f, 0.0f, 1.0f}, 50.0f);
    p2->Name = "Red Light";
    auto p3 = CreatePointLight({0,0,7}, {0.0f, 1.0f, 0.0f, 1.0f}, 50.0f);
    p3->Name = "Green Light";
    auto p = CreatePointLight({5,0,5}, {0.0f, 0.0f, 1.0f, 1.0f}, 50.0f);
    p->Name = "Blue Light";
    auto d = CreateDirectionalLight(vec4(1));

    scene.AddSceneObject(p);
    scene.AddSceneObject(p2);
    scene.AddSceneObject(p3);
    scene.AddSceneObject(d);
    scene.AddSceneObject(cameraObject);
    scene.AddSceneObject(sphere1);
    sphere2->Name = "Sphere 2";
    scene.AddSceneObject(sphere2);
    sphere3->Name = "Sphere 2";
    scene.AddSceneObject(sphere3);
    scene.AddSceneObject(skybox);
    if(!testDeferred)
    {
        auto createRenderpass = [&](){
            auto rp = Renderpass::Create()
                .NewSubpass("Skybox", SubpassFlags::DISABLE_DEPTHMASK)
                .DrawMesh(skybox->GetComponent<MeshComponent>())
                .NewSubpass("Forward pass")
                .DrawMesh(sphere1->GetComponent<MeshComponent>())
                .DrawMesh(sphere2->GetComponent<MeshComponent>())
                .DrawMesh(sphere3->GetComponent<MeshComponent>())
                .Build();
            return rp;
        };
        renderer->SetRenderpassReconstructionCallback(createRenderpass);
    }
    if(testDeferred)
    {
        // DEFERRED SHADING TEST
        vec2 winDims = Renderer::GetInstance()->GetWindowDimensions();
        auto gBuffer = Framebuffer::Create(winDims.x, winDims.y)
                        .WithColorbuffer("position", GL_RGBA16F)
                        .WithColorbuffer("normal", GL_RGBA16F)
                        .WithColorbuffer("reflectance", GL_RGBA16F)
                        .WithColorbuffer("albedospec", GL_RGBA)
                        .WithDepthbuffer("depth")
                        .Build();

        Shader* light = Shader::Create("Light").WithPPVertexFunctions().WithDeferredPBRLighting().Build();
        light->AllocateBuffers(10);
        Material* mat = light->CreateMaterial();
        mat->SetTexture("gBuffer.position", gBuffer->GetColorbuffer("position"));
        mat->SetTexture("gBuffer.normal", gBuffer->GetColorbuffer("normal"));
        mat->SetTexture("gBuffer.reflectance", gBuffer->GetColorbuffer("reflectance"));
        mat->SetTexture("gBuffer.albedoSpec", gBuffer->GetColorbuffer("albedospec"));

        auto quad = Mesh::GetQuad(light);
        auto postprocessingQuad = new SceneObject();
        postprocessingQuad->Name = "PostProcessingQuad";
        scene.AddSceneObject(postprocessingQuad);
        auto ppmp = postprocessingQuad->AddComponent<MeshComponent>();
        ppmp->SetMesh(quad);
        ppmp->SetMaterial(mat);

        auto createRenderpass = [&](){
            auto rp = Renderpass::Create()
                .NewSubpass("Skybox", SubpassFlags::DISABLE_DEPTHMASK)
                .DrawMesh(skybox->GetComponent<MeshComponent>())
                .NewSubpass("Geometry")
                .UseFramebuffer(gBuffer)
                .DrawMesh(sphere1->GetComponent<MeshComponent>())
                .DrawMesh(sphere2->GetComponent<MeshComponent>())
                .DrawMesh(sphere3->GetComponent<MeshComponent>())
                .NewSubpass("Lighting")
                .UseFramebuffer(Framebuffer::GetDefault())
                .DrawMesh(ppmp)
                .Build();
            return rp;
        };

        WindowManager::GetInstance()->RegisterWindowResizeCallback([&](int w, int h){
            winDims = Renderer::GetInstance()->GetWindowDimensions();
            delete gBuffer;
            gBuffer = Framebuffer::Create(winDims.x, winDims.y)
                    .WithColorbuffer("position", GL_RGBA16F)
                    .WithColorbuffer("normal", GL_RGBA16F)
                    .WithColorbuffer("freflectance", GL_RGBA16F)
                    .WithColorbuffer("albedospec", GL_RGBA)
                    .WithDepthbuffer("depth")
                    .Build();
        });

        renderer->SetRenderpassReconstructionCallback(createRenderpass);
    }


    renderer->SetScene(&scene);
    renderer->RenderLoop();
    return 0;
}

int main()
{
    scene2(true);
}