#include "renderer/Renderer.hpp"
#include "renderer/Mesh.hpp"
#include "renderer/Scene.hpp"
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

SceneObject* CreatePlane(Shader* PBRShader, string name = "Sphere")
{
    Mesh* sphere = Mesh::GetPlane(2,2, PBRShader, 250.0f); //Mesh::GetSphere(PBRShader);
    SceneObject* object = new SceneObject();
    MeshComponent* meshcomp = object->AddComponent<MeshComponent>();
    Material* mat = PBRShader->CreateMaterial();
    Texture* albedo =   Utilities::ImportTexture(GetAbsoluteResourcesPath("\\PBR_materials\\[4K]Tiles58\\Tiles58_col.jpg"), GL_TEXTURE_2D);
    Texture* metallic = Utilities::ImportTexture(GetAbsoluteResourcesPath("\\PBR_materials\\[4K]Tiles58\\Tiles58_met.jpg"), GL_TEXTURE_2D);
    Texture* roughness =Utilities::ImportTexture(GetAbsoluteResourcesPath("\\PBR_materials\\[4K]Tiles58\\Tiles58_rgh.jpg"), GL_TEXTURE_2D);
    Texture* normal =   Utilities::ImportTexture(GetAbsoluteResourcesPath("\\PBR_materials\\[4K]Tiles58\\Tiles58_nrm.jpg"), GL_TEXTURE_2D);
    // Texture* ao =       Utilities::ImportTexture(GetAbsoluteResourcesPath("\\PBR_materials\\[4K]Planks16\\Planks16_AO.jpg"), GL_TEXTURE_2D);
    object->Name = name;
    object->transform.position = {50.0f, 3.5f, 50.0f};
    mat->SetTexture("albedo", albedo);
    mat->SetTexture("metallic", metallic);
    mat->SetTexture("roughness", roughness);
    // mat->SetTexture("ambient", ao);
    mat->GetUniform("hasAO")->i = 0;
    mat->SetNormalMap(normal);
    // mat->GetUniform("PBR.Albedo")->f4 = {0.8f, 0.8f, 0.8f, 1.0f};
    // mat->GetUniform("PBR.Roughness")->f = 0.550f;
    // mat->GetUniform("PBR.Metallic")->f = 0.630f;
    mat->GetUniform("PBR.F0")->f4 = {0.24, 0.24, 0.24, 1.0f};
    mat->GetUniform("Renderer.surface.EnvironmentReflectivity")->f = 0.0;
    meshcomp->SetMesh(sphere);
    meshcomp->SetMaterial(mat);

    return object;
}

SceneObject* CreateSphere(vec3 position, vec3 rotation, Shader* PBRShader)
{
    static Mesh* sphere = Mesh::GetSphere(PBRShader);
    SceneObject* object = new SceneObject();
    MeshComponent* meshcomp = object->AddComponent<MeshComponent>();
    Material* mat = PBRShader->CreateMaterial();
    static Texture* albedo =   Utilities::ImportTexture(GetAbsoluteResourcesPath("\\PBR_materials\\[4K]Tiles58\\Tiles58_col.jpg"), GL_TEXTURE_2D);
    static Texture* metallic = Utilities::ImportTexture(GetAbsoluteResourcesPath("\\PBR_materials\\[4K]Tiles58\\Tiles58_met.jpg"), GL_TEXTURE_2D);
    static Texture* roughness =Utilities::ImportTexture(GetAbsoluteResourcesPath("\\PBR_materials\\[4K]Tiles58\\Tiles58_rgh.jpg"), GL_TEXTURE_2D);
    static Texture* normal =   Utilities::ImportTexture(GetAbsoluteResourcesPath("\\PBR_materials\\[4K]Tiles58\\Tiles58_nrm.jpg"), GL_TEXTURE_2D);

    object->Name = "Sphere";
    object->transform.position = position;
    object->transform.rotation = rotation;
    mat->SetTexture("albedo", albedo);
    mat->SetTexture("metallic", metallic);
    mat->SetTexture("roughness", roughness);
    int has = 0;
    // mat->SetUniform<int>("hasAO",has);
    mat->SetNormalMap(normal);
    // mat->SetUniform<glm::vec4>("PBR.F0", glm::vec4(0.24, 0.24, 0.24, 1.0f));
    // float f = 0.0f;
    // mat->SetUniform<float>("EnvironmentReflectivity", f);
    meshcomp->SetMesh(sphere);
    meshcomp->SetMaterial(mat);
    return object;
}


SceneObject* CreateTerrain(std::string path, vec3 position, vec3 rotation, Shader* PBRShader)
{
    static Mesh* sphere = Mesh::FromHeightmap(path, 1000.0f, 255.0f, PBRShader);
    SceneObject* object = new SceneObject();
    MeshComponent* meshcomp = object->AddComponent<MeshComponent>();
    Material* mat = PBRShader->CreateMaterial();
    static Texture* albedo =   Utilities::ImportTexture(GetAbsoluteResourcesPath("\\PBR_materials\\[4K]Rock\\Rock_col.jpg"), GL_TEXTURE_2D);
    static Texture* metallic = Utilities::ImportTexture(GetAbsoluteResourcesPath("\\PBR_materials\\[4K]Rock\\Rock_met.jpg"), GL_TEXTURE_2D);
    static Texture* roughness =Utilities::ImportTexture(GetAbsoluteResourcesPath("\\PBR_materials\\[4K]Rock\\Rock_rgh.jpg"), GL_TEXTURE_2D);
    static Texture* normal =   Utilities::ImportTexture(GetAbsoluteResourcesPath("\\PBR_materials\\[4K]Rock\\Rock_nrm.jpg"), GL_TEXTURE_2D);
    object->Name = "Terrain";
    object->transform.position = position;
    object->transform.rotation = rotation;
    mat->SetTexture("albedo", albedo);
    mat->SetTexture("metallic", metallic);
    mat->SetTexture("roughness", roughness);
    mat->GetUniform("hasAO")->i = 0;
    mat->SetNormalMap(normal);
    // mat->GetUniform("PBR.Albedo")->f4 = {0.8f, 0.8f, 0.8f, 1.0f};
    // mat->GetUniform("PBR.Roughness")->f = 0.550f;
    // mat->GetUniform("PBR.Metallic")->f = 0.630f;
    mat->GetUniform("PBR.F0")->f4 = {0.24, 0.24, 0.24, 1.0f};
    mat->GetUniform("Renderer.surface.EnvironmentReflectivity")->f = 0.0;
    meshcomp->SetMesh(sphere);
    meshcomp->SetMaterial(mat);
    return object;
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

int scene1()
{
    Renderer* renderer = Renderer::GetInstance();
    Scene scene = Scene();
    Texture* back = Utilities::ImportTexture(GetAbsoluteResourcesPath("\\texture\\skybox\\back.tga"));
    Texture* front = Utilities::ImportTexture(GetAbsoluteResourcesPath("\\texture\\skybox\\front.tga"));
    Texture* left = Utilities::ImportTexture(GetAbsoluteResourcesPath("\\texture\\skybox\\left.tga"));
    Texture* right = Utilities::ImportTexture(GetAbsoluteResourcesPath("\\texture\\skybox\\right.tga"));
    Texture* top = Utilities::ImportTexture(GetAbsoluteResourcesPath("\\texture\\skybox\\top.tga"));
    Texture* bot = Utilities::ImportTexture(GetAbsoluteResourcesPath("\\texture\\sand_bot2.jpg"));
    Texture* skyboxTexture = new Texture(right, left, top, bot, back, front);

    Shader* gerstnerShader = Shader::GetGerstnerWaveShader_PBR();
    Shader* PBRShader = Shader::GetTexturesPBRShader();
    Mesh* waveMesh = Mesh::GetPlane(1000, 1000, gerstnerShader, 100.0f);
    

    Texture* normalMap = Utilities::ImportTexture(GetAbsoluteResourcesPath("\\texture\\water_norm_map.png"), GL_TEXTURE_2D);

    Texture* albedo = Utilities::ImportTexture(GetAbsoluteResourcesPath("\\texture\\aa_beauty_and_the_sun.png"), GL_TEXTURE_2D);
    SceneObject *obj = new SceneObject();
    obj->Name = "Water";
    auto mp = obj->AddComponent<MeshComponent>();
    auto wm = obj->AddComponent<WaveManagerComponent>();

    wm->AddDirectionalWave(105.0f, 1.0f, 10.0f, 10.0f);
    wm->AddDirectionalWave(155.0f, 1.0f, 10.0f, 10.0f);
    wm->AddDirectionalWave(125.0f, 1.0f, 10.0f, 10.0f);
    wm->AddDirectionalWave(25.0f, 1.0f, 10.0f, 10.0f);


    mp->SetMesh(waveMesh);
    Material* mat = gerstnerShader->CreateMaterial();
    mat->SetNormalMap(normalMap);
    mat->SetTexture("albedo", albedo);
    if(mat->GetUniform("PBR.Albedo") != nullptr)
    {
        mat->GetUniform("PBR.Albedo")->f4 = {0.3f, 0.3f, 0.3f, 1.0f};
        mat->GetUniform("PBR.Roughness")->f = 0.250;
        mat->GetUniform("PBR.Metallic")->f = 0.630;
        mat->GetUniform("PBR.F0")->f4 = {0.2f, 0.2f, 0.2f, 1.0f};
        mat->GetUniform("Renderer.surface.EnvironmentReflectivity")->f = 0.230f;
    }

    mp->SetMaterial(mat);
    wm->SetGerstnerMaterial(mat);

    SceneObject* cameraObject = new SceneObject();
    cameraObject->Name = "Camera";
    cameraObject->transform.position = glm::vec3(0, 5, 0);
    auto cam = cameraObject->AddComponent<CameraComponent>();


    auto skybox = cameraObject->AddComponent<SkyboxComponent>();
    skybox->SetSkyboxTexture(skyboxTexture);

    
    auto mov = cameraObject->AddComponent<MovementComponent>();
    mov->SetCamera(cam);
    mov->SetWaveManager(wm);
    SceneObject* light = new SceneObject();
    light->Name = "Directional Light";
    light->transform.position.y = 5.0f;
    light->transform.rotation = {-135.0f, 0.0f, 0.0f};
    auto lcomp = light->AddComponent<LightComponent>();
    lcomp->SetType(LightType::DIRECTIONAL);
    

    SceneObject* pointlight = CreatePointLight({-5,5,5}, {0,0,1,1}, 30.0f);
    SceneObject* pointlight2 = CreatePointLight({0,5,5}, {0,1,0,1}, 30.0f);
    SceneObject* pointlight3 = CreatePointLight({5,5,5}, {1,0,0,1}, 30.0f);
    SceneObject* pointlight4 = CreatePointLight({-10,5,-10}, {1,1,1,1}, 30.0f);
    

    auto cube1 = CreateSphere(vec3(-5.0f, 5.0f, 0.0f), vec3(45.0f, -80.0f, 0.0f), PBRShader);
    auto cube2 = CreateSphere(vec3(5.0f, 5.0f, 0.0f), vec3(-45.0f, 80.0f, 0.0f), PBRShader);
    auto cube3 = CreateSphere(vec3(0.0f, 5.0f, 0.0f), vec3(0.0f, 0.0f, 0.0f), PBRShader);

    auto sphere = CreatePlane(PBRShader, "planks");

    auto slerp = cube3->AddComponent<SlerpComponent>();
    slerp->SetTransforms(&cube1->transform, &cube2->transform);

    scene.AddSceneObject(cube1);
    scene.AddSceneObject(cube2);
    scene.AddSceneObject(cube3);
    scene.AddSceneObject(sphere);
    
    lcomp->SetDebugDrawDirectionEnabled();
    scene.AddSceneObject(obj);
    scene.AddSceneObject(cameraObject);
    scene.AddSceneObject(light);
    scene.AddSceneObject(pointlight);
    scene.AddSceneObject(pointlight2);
    scene.AddSceneObject(pointlight3);
    scene.AddSceneObject(pointlight4);
    // scene.AddSceneObject(CreateTerrain(GetAbsoluteResourcesPath("\\heightmaps\\fromgoogle.png"), {0,0,0}, {0,0,0}));
    renderer->SetScene(&scene);
    renderer->RenderLoop();
    return 0;
}

int scene2()
{
    Renderer* renderer = Renderer::GetInstance();
    Scene scene = Scene();
    SceneObject* cameraObject = new SceneObject();
    cameraObject->Name = "Camera";
    cameraObject->transform.position = glm::vec3(0, 0, -5);
    auto cam = cameraObject->AddComponent<CameraComponent>();
    
    SceneObject* quad = new SceneObject();
    Shader* shader = new Shader(GetAbsoluteResourcesPath("\\shaders\\testing\\vertex.vert"), GetAbsoluteResourcesPath("\\shaders\\testing\\fragment.frag"));
    Mesh* quadMesh = Mesh::GetSphere(shader);
    auto mp = quad->AddComponent<MeshComponent>();
    mp->SetMesh(quadMesh);
    mp->SetMaterial(shader->CreateMaterial());

    // Shader* PBRShader = Shader::GetTexturesPBRShader();

    // auto sphere = CreateSphere(vec3(0), vec3(0), PBRShader);

    auto p = CreatePointLight({0,0,0}, {1.0f, 0.0f, 0.0f, 1.0f}, 10.0f);
    auto d = CreateDirectionalLight(vec4(1));

    scene.AddSceneObject(p);
    scene.AddSceneObject(d);
    scene.AddSceneObject(cameraObject);
    // scene.AddSceneObject(sphere);
    auto winMan = WindowManager::GetInstance();
    winMan->UnlockCursor(winMan->GetActiveWindow());

    renderer->SetScene(&scene);
    renderer->RenderLoop();
    return 0;
}

int main()
{
    scene2();
}