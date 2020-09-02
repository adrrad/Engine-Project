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
using namespace Engine::Core;

Texture* lightbulbIcon;

GameObject* CreateSphere(vec3 position, Shader* shader)
{
    static Texture* albedo =   Utilities::ImportTexture(GetAbsoluteResourcesPath("\\PBR_materials\\[4K]Tiles58\\Tiles58_col.jpg"), GL_TEXTURE_2D);
    static Texture* metallic = Utilities::ImportTexture(GetAbsoluteResourcesPath("\\PBR_materials\\[4K]Tiles58\\Tiles58_met.jpg"), GL_TEXTURE_2D);
    static Texture* roughness =Utilities::ImportTexture(GetAbsoluteResourcesPath("\\PBR_materials\\[4K]Tiles58\\Tiles58_rgh.jpg"), GL_TEXTURE_2D);
    static Texture* normal =   Utilities::ImportTexture(GetAbsoluteResourcesPath("\\PBR_materials\\[4K]Tiles58\\Tiles58_nrm.jpg"), GL_TEXTURE_2D);
    Mesh* sphereMesh =  Mesh::GetSphere();

    GameObject* sphere = new GameObject();
    sphere->Name = "Sphere";
    sphere->transform.position = position;

    auto mp = sphere->AddComponent<MeshComponent>();
    Material* mat = shader->CreateMaterial();
    vec3 f = vec3(0.24);
    mat->SetProperty<vec3>("PBRProperties", "F0", f);
    mat->SetTexture("textures.albedo", albedo);
    mat->SetTexture("textures.metallic", metallic);
    mat->SetTexture("textures.roughness", roughness);
    mat->SetTexture("textures.normal", normal);
    mp->SetMesh(sphereMesh);
    mp->SetMaterial(mat);
    return sphere;
}

GameObject* CreateQuad(vec3 position, Shader* shader)
{
    static Texture* albedo =   Utilities::ImportTexture(GetAbsoluteResourcesPath("\\PBR_materials\\[4K]Tiles58\\Tiles58_col.jpg"), GL_TEXTURE_2D);
    static Texture* metallic = Utilities::ImportTexture(GetAbsoluteResourcesPath("\\PBR_materials\\[4K]Tiles58\\Tiles58_met.jpg"), GL_TEXTURE_2D);
    static Texture* roughness =Utilities::ImportTexture(GetAbsoluteResourcesPath("\\PBR_materials\\[4K]Tiles58\\Tiles58_rgh.jpg"), GL_TEXTURE_2D);
    static Texture* normal =   Utilities::ImportTexture(GetAbsoluteResourcesPath("\\PBR_materials\\[4K]Tiles58\\Tiles58_nrm.jpg"), GL_TEXTURE_2D);
    Mesh* sphereMesh =  Mesh::GetQuad();

    GameObject* sphere = new GameObject();
    sphere->Name = "Quad";
    sphere->transform.position = position;

    auto mp = sphere->AddComponent<MeshComponent>();
    Material* mat = shader->CreateMaterial();
    vec3 f = vec3(0.24);
    mat->SetProperty<vec3>("PBRProperties", "F0", f);
    mat->SetTexture("textures.albedo", albedo);
    mat->SetTexture("textures.metallic", metallic);
    mat->SetTexture("textures.roughness", roughness);
    mat->SetTexture("textures.normal", normal);
    mp->SetMesh(sphereMesh);
    mp->SetMaterial(mat);
    return sphere;
}

void AddBillboard(GameObject* obj, Texture* texture)
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

GameObject* CreateUnlitQuad(vec3 position, Shader* shader)
{
    static Texture* icon = Utilities::ImportTexture(GetAbsoluteResourcesPath("\\icons\\lightbulb_outline.png"), GL_TEXTURE_2D);
    Mesh* quadMesh =  Mesh::GetQuad();

    GameObject* quad = new GameObject();
    quad->Name = "Quad";
    quad->transform.position = position;

    auto mp = quad->AddComponent<MeshComponent>();
    Material* mat = shader->CreateMaterial();
    mat->SetTexture("billboard.texture", icon);
    mp->SetMesh(quadMesh);
    mp->SetMaterial(mat);
    return quad;
}

GameObject* CreateIsland(vec3 position, Shader* shader)
{
    static Texture* albedo =   Utilities::ImportTexture(GetAbsoluteResourcesPath("\\PBR_materials\\Ground\\Ground_Albedo.jpg"), GL_TEXTURE_2D);
    static Texture* metallic = Utilities::ImportTexture(GetAbsoluteResourcesPath("\\PBR_materials\\[4K]Tiles58\\Tiles58_met.jpg"), GL_TEXTURE_2D);
    static Texture* roughness =Utilities::ImportTexture(GetAbsoluteResourcesPath("\\PBR_materials\\Ground\\Ground_Roughness.jpg"), GL_TEXTURE_2D);
    static Texture* normal =   Utilities::ImportTexture(GetAbsoluteResourcesPath("\\PBR_materials\\Ground\\Ground_Normal.jpg"), GL_TEXTURE_2D);
    Mesh* sphereMesh =  Mesh::FromHeightmap(
        GetAbsoluteResourcesPath("\\heightmaps\\island_resized_128.png"),
        1000,
        200,
        1000);

    GameObject* sphere = new GameObject();
    sphere->Name = "Island Terrain";
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

GameObject* CreateSkybox(Shader* shader, Material* mat)
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
    auto sb = skybox ->AddComponent<SkyboxComponent>();
    mp->SetMesh(cubeMesh);
    mat->SetTexture("skybox.texture", skyboxTexture);
    mp->SetMaterial(mat);
    return skybox;
}

GameObject* CreatePointLight(vec3 position, vec4 colour, float radius)
{
    GameObject* pointlight = new GameObject();
    pointlight->Name = "Point Light";
    pointlight->transform.position = position;
    auto plight = pointlight->AddComponent<LightComponent>();
    plight->SetType(LightType::POINT);
    plight->PointLight().Colour = colour;
    plight->PointLight().Radius = radius;

    //Add light icon
    AddBillboard(pointlight, lightbulbIcon);
    return pointlight;
}

GameObject* CreateDirectionalLight(vec4 colour)
{
    GameObject* light = new GameObject();
    light->Name = "Directional Light";
    light->transform.position.y = 5.0f;
    light->transform.rotation = {-135.0f, 0.0f, 0.0f};
    auto lcomp = light->AddComponent<LightComponent>();
    lcomp->SetType(LightType::DIRECTIONAL);
    lcomp->SetColour(colour);
    AddBillboard(light, lightbulbIcon);
    return light;
}

int scene2(bool testDeferred)
{
    Renderer* renderer = Renderer::GetInstance();
    Scene scene = Scene();
    lightbulbIcon = Utilities::ImportTexture(GetAbsoluteResourcesPath("\\icons\\lightbulb_outline.png"), GL_TEXTURE_2D);

    GameObject* cameraObject = new GameObject();
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
    sphere1->transform.rotation = {0, 0, 0};
    auto sphere3 = CreateSphere({3,0,0}, testDeferred ? deferred : shader);
    sphere3->transform.rotation = {0, 0, 90};
    auto sphere2 = CreateSphere({0,0,0}, testDeferred ? deferred : shader);
    auto slerp = sphere2->AddComponent<SlerpComponent>();
    slerp->SetTransforms(&sphere1->transform, &sphere3->transform);
    sphere2->transform.SetParent(&sphere1->transform);
    auto island = CreateIsland(vec3(0, -95, 0), testDeferred ? deferred : shader);
    auto watah = CreateQuad(vec3(0,-80, 0), testDeferred ? deferred : shader);
    watah->transform.rotation.x = 90.0f;
    watah->transform.scale = vec3(1000, 1000, 1);
    //SKYBOX
    Shader* skyShader = Shader::Create("Skybox").WithSkyboxVertexFunctions().WithSkybox(testDeferred).Build();
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
    scene.AddGameObject(watah);
    scene.AddGameObject(island);
    scene.AddGameObject(p);
    scene.AddGameObject(p2);
    scene.AddGameObject(p3);
    scene.AddGameObject(d);
    scene.AddGameObject(cameraObject);
    scene.AddGameObject(sphere1);
    sphere2->Name = "Sphere 2";
    scene.AddGameObject(sphere2);
    sphere3->Name = "Sphere 2";
    scene.AddGameObject(sphere3);
    scene.AddGameObject(skybox);
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
        auto postprocessingQuad = new GameObject();
        postprocessingQuad->Name = "PostProcessingQuad";
        scene.AddGameObject(postprocessingQuad);
        auto ppmp = postprocessingQuad->AddComponent<MeshComponent>();
        ppmp->SetMesh(quad);
        ppmp->SetMaterial(mat);

        auto createRenderpass = [&](){
            auto rp = Renderpass::Create()
                .NewSubpass("Geometry")
                .UseFramebuffer(gBuffer)
                .DrawMesh(sphere1->GetComponent<MeshComponent>())
                .DrawMesh(sphere2->GetComponent<MeshComponent>())
                .DrawMesh(sphere3->GetComponent<MeshComponent>())
                .DrawMesh(island->GetComponent<MeshComponent>())
                .DrawMesh(watah->GetComponent<MeshComponent>())
                .NewSubpass("Lighting")
                .UseFramebuffer(lightBuffer)
                .DrawMesh(ppmp)
                .NewSubpass("Skybox", SubpassFlags::DISABLE_DEPTHMASK)
                .UseFramebuffer(Framebuffer::GetDefault())
                .DrawMesh(skybox->GetComponent<MeshComponent>())
                .NewSubpass("Overlay", SubpassFlags::DISABLE_DEPTHMASK | SubpassFlags::ENABLE_BLENDING)
                .DrawMesh(p->GetComponent<MeshComponent>())
                .DrawMesh(p2->GetComponent<MeshComponent>())
                .DrawMesh(p3->GetComponent<MeshComponent>())
                .DrawMesh(d->GetComponent<MeshComponent>())
                .Build();
            return rp;
        };

        WindowManager::GetInstance()->RegisterWindowResizeCallback([&](int w, int h){
            winDims = Renderer::GetInstance()->GetWindowDimensions();
            lightBuffer->Rebuild(w,h);
            gBuffer->Rebuild(w, h);
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