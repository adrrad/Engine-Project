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
    Mesh* waveMesh = Mesh::GetParticlePlane(1000, 1000, shader, 200.0f);

    Shader* phong = Shader::GetPhongShader();
    Shader* PBR = Shader::GetPBRShader();
    Mesh* cube = Mesh::GetSphere(phong);
    Mesh* sphere = Mesh::GetSphere(PBR);
    Texture* texture = Utilities::ImportTexture(GetAbsoluteResourcesPath("\\texture\\water.jpg"), GL_TEXTURE_2D);
    SceneObject *sphereObject = new SceneObject();
    SceneObject *cubeObject = new SceneObject();
    sphereObject->Name = "Sphere";
    // sphereObject->transform.rotation.y = 45.0f;
    sphereObject->transform.position.y = 15.0f;
    cubeObject->Name = "Cube";
    // cubeObject->transform.rotation.y = 45.0f;
    cubeObject->transform.position = {2.0f, 15.0f, 0.0f};
    auto cubemp = cubeObject->AddComponent<MeshComponent>();
    cubemp->SetMesh(cube);
    cubemp->SetMaterial(new Material(phong));
    auto spheremp = sphereObject->AddComponent<MeshComponent>();
    spheremp->SetMesh(sphere);
    spheremp->SetMaterial(new Material(PBR));
    

    SceneObject *obj = new SceneObject();
    obj->Name = "Wave Plane";
    auto mp = obj->AddComponent<MeshComponent>();
    auto wm = obj->AddComponent<WaveManagerComponent>();
    
    // wm->AddFloatingObject(cubeObject);

    // wm->AddCircularWave(glm::vec2(100.0f), 0.1f, 10.0f, 0.1f);
    // wm->AddCircularWave(glm::vec2(100.0f, 100.0f), 0.1f, 10.0f, 0.24f);
    // wm->AddCircularWave(glm::vec2(100.0f, -1000.0f), 0.1f, 10.0f, 0.38f);
    wm->AddDirectionalWave(25.0f, 1.0f, 10.0f, 10.0f);
    wm->AddDirectionalWave(115.0f, 0.1f, 25.0f, 6.0f, 1.0f);
    wm->AddDirectionalWave(205.0f, 0.05f, 6.0f, 1.0f, 10.0f);
    wm->AddDirectionalWave(245.0f, 0.05f, 8.6f, 2.0f, 5.0f);
    wm->AddDirectionalWave(50.0f, 0.01f, 5.3f, 3.0f, 2.0f);
    wm->AddDirectionalWave(205.0f, 0.01f, 25.0f, 10.0f);
    // AddCircularWave(glm::vec3 center, float amplitude, float wavelength, float speed, float power, float range, float lifespan)
    // wm->AddCircularWave(glm::vec2(1000.0f, -1000.0f), 0.0004f, 0.8f, 0.05f);
    // wm->AddCircularWave(glm::vec2(1000.0f, 1000.0f), 0.0006f, 0.5f, 0.7f);
    // wm->AddCircularWave(glm::vec2(-1000.0f, 1000.0f), 0.0005f, 0.3f, 0.12f);
    // wm->AddCircularWave(glm::vec2(-1000.0f, -1000.0f), 0.007f, 0.6f, 0.26f);

    mp->SetMesh(waveMesh);
    Material* mat = new Material(shader);
    mat->SetTexture(texture);
    mp->SetMaterial(mat);
    // mp->SetTexture(texture);
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
    light->transform.rotation.x = 45.0f;
    auto lcomp = light->AddComponent<DirectionalLightComponent>();
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