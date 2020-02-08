#include "renderer/Renderer.hpp"
#include "renderer/Mesh.hpp"
#include "renderer/Scene.hpp"
#include "utilities/Utilities.hpp"

#include "components/CameraComponent.hpp"
#include "components/MovementComponent.hpp"
#include "components/DirectionalLightComponent.hpp"
#include "components/MeshComponent.hpp"
#include "components/WaveManagerComponent.hpp"
#include "components/SkyboxComponent.hpp"

#include <iostream>

using namespace std;
using namespace Rendering;
using namespace Components;
using namespace Utilities;

int main()
{
    Renderer* renderer = Renderer::GetInstance();
    Scene scene = Scene();
    
    Texture* back = Utilities::ImportTexture(GetAbsoluteResourcesPath("\\texture\\skybox\\back.tga"));
    Texture* front = Utilities::ImportTexture(GetAbsoluteResourcesPath("\\texture\\skybox\\front.tga"));
    Texture* left = Utilities::ImportTexture(GetAbsoluteResourcesPath("\\texture\\skybox\\left.tga"));
    Texture* right = Utilities::ImportTexture(GetAbsoluteResourcesPath("\\texture\\skybox\\right.tga"));
    Texture* top = Utilities::ImportTexture(GetAbsoluteResourcesPath("\\texture\\skybox\\top.tga"));
    // Texture* bot = Utilities::ImportTexture(GetAbsoluteResourcesPath("\\texture\\skybox\\bottom.tga"));
    Texture* bot = Utilities::ImportTexture(GetAbsoluteResourcesPath("\\texture\\sand_bot2.jpg"));
    Texture* skyboxTexture = new Texture(right, left, top, bot, back, front);

    Shader* shader = Shader::GetGerstnerWaveShader_PBR();
    Mesh* waveMesh = Mesh::GetPlane(1000, 1000, shader, 100.0f);

    Shader* phong = Shader::GetPhongShader();
    Shader* PBR = Shader::GetPBRShader();
    Mesh* cube = Mesh::GetCube(phong);
    Mesh* sphere = Mesh::GetSphere(PBR);
    Texture* texture = Utilities::ImportTexture(GetAbsoluteResourcesPath("\\texture\\aa_beauty_and_the_sun.png"), GL_TEXTURE_2D);

    Texture* brickwall = Utilities::ImportTexture(GetAbsoluteResourcesPath("\\texture\\water.jpg"), GL_TEXTURE_2D);
    Texture* normalMap = Utilities::ImportTexture(GetAbsoluteResourcesPath("\\texture\\normal_map.jpeg"), GL_TEXTURE_2D);
    SceneObject *sphereObject = new SceneObject();
    SceneObject *cubeObject = new SceneObject();
    sphereObject->Name = "Sphere";
    sphereObject->transform.position.y = 5.0f;
    cubeObject->transform.position.z = -5.0f;
    cubeObject->transform.position.y = 5.0f;
    cubeObject->Name = "Cube";
    // cubeObject->transform.rotation.y = 45.0f;
    // cubeObject->transform.position = {2.0f, 15.0f, 0.0f};
    auto cubemp = cubeObject->AddComponent<MeshComponent>();
    cubemp->SetMesh(cube);
    auto cubemat = new Material(phong);
    // cubemat->SetTexture(texture);
    cubemp->SetMaterial(cubemat);
    auto spheremp = sphereObject->AddComponent<MeshComponent>();
    spheremp->SetMesh(sphere);
    auto sphereMat = new Material(PBR);
    if(sphereMat->GetUniform("PBR.Albedo") != nullptr)
    {
        sphereMat->GetUniform("PBR.Albedo")->f4 = {1.0f, 1.0f, 1.0f, 1.0f};
        sphereMat->GetUniform("PBR.Roughness")->f = 0.1;
        sphereMat->GetUniform("PBR.Metallic")->f = 0.7;
        sphereMat->GetUniform("PBR.F0")->f4 = {0.96f, 0.96f, 0.97f, 1.0f};
        
    }
    sphereMat->GetUniform("Renderer.surface.EnvironmentReflectivity")->f = 0;
    sphereMat->SetTexture(texture);
    spheremp->SetMaterial(sphereMat);
    

    SceneObject *obj = new SceneObject();
    obj->Name = "Wave Plane";
    auto mp = obj->AddComponent<MeshComponent>();
    auto wm = obj->AddComponent<WaveManagerComponent>();
    
    wm->AddDirectionalWave(25.0f, 1.0f, 10.0f, 10.0f);
    // wm->AddDirectionalWave(115.0f, 0.1f, 25.0f, 6.0f, 1.0f);
    // wm->AddDirectionalWave(205.0f, 0.05f, 6.0f, 1.0f, 10.0f);
    // wm->AddDirectionalWave(245.0f, 0.05f, 8.6f, 2.0f, 5.0f);
    // wm->AddDirectionalWave(50.0f, 0.01f, 5.3f, 3.0f, 2.0f);
    // wm->AddDirectionalWave(205.0f, 0.01f, 25.0f, 10.0f);


    mp->SetMesh(waveMesh);
    Material* mat = new Material(shader);
    // mat->SetTexture(brickwall);
    mat->SetNormalMap(normalMap);
    if(mat->GetUniform("PBR.Albedo") != nullptr)
    {
        mat->GetUniform("PBR.Albedo")->f4 = {1.0f, 1.0f, 1.0f, 1.0f};
        mat->GetUniform("PBR.Roughness")->f = 0.7;
        mat->GetUniform("PBR.Metallic")->f = 0.1;
        mat->GetUniform("PBR.F0")->f4 = {0.96f, 0.96f, 0.97f, 1.0f};
    }

    mat->GetUniform("Renderer.surface.EnvironmentReflectivity")->f = 1.0f;
    mp->SetMaterial(mat);
    mp->SetTexture(brickwall);
    wm->SetGerstnerMaterial(mat);
    obj->transform.position = glm::vec3(0.0f, -0.5f, 0.0f);

    
    // Texture* back = Utilities::ImportTexture(GetAbsoluteResourcesPath("\\texture\\sor_sea\\sea_bk.JPG"));
    // Texture* front = Utilities::ImportTexture(GetAbsoluteResourcesPath("\\texture\\sor_sea\\sea_ft.JPG"));
    // Texture* left = Utilities::ImportTexture(GetAbsoluteResourcesPath("\\texture\\sor_sea\\sea_lf.JPG"));
    // Texture* right = Utilities::ImportTexture(GetAbsoluteResourcesPath("\\texture\\sor_sea\\sea_rt.JPG"));
    // Texture* top = Utilities::ImportTexture(GetAbsoluteResourcesPath("\\texture\\sor_sea\\sea_up.JPG"));
    // Texture* bot = Utilities::ImportTexture(GetAbsoluteResourcesPath("\\texture\\sor_sea\\sea_dn.JPG"));


    SceneObject* cameraObject = new SceneObject();
    cameraObject->Name = "Camera";
    cameraObject->transform.position = glm::vec3(0, 5, 0);
    auto cam = cameraObject->AddComponent<CameraComponent>();
    cam->SetMain();
    try
    {
        auto skybox = cameraObject->AddComponent<SkyboxComponent>();
        skybox->SetSkyboxTexture(skyboxTexture);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
    auto mov = cameraObject->AddComponent<MovementComponent>();
    mov->SetCamera(cam);
    mov->SetWaveManager(wm);
    SceneObject* light = new SceneObject();
    light->Name = "Directional Light";
    light->transform.position.y = 5.0f;
    light->transform.rotation = {90.0f, 0.0f, 0.0f};
    auto lcomp = light->AddComponent<DirectionalLightComponent>();
    // lcomp->SetColour({0.97f, 0.843f, 0.109f, 0.0f});
    lcomp->SetDebugDrawDirectionEnabled();
    scene.AddSceneObject(sphereObject);
    scene.AddSceneObject(cubeObject);
    scene.AddSceneObject(obj);
    scene.AddSceneObject(cameraObject);
    scene.AddSceneObject(light);
    renderer->SetScene(&scene);
    renderer->RenderLoop();
    return 0;
}