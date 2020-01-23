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

    Shader* shader = Shader::GetGerstnerWaveShader();
    Mesh* waveMesh = Mesh::GetParticlePlane(1000, 1000, shader, 200.0f);

    Shader* phong = Shader::GetPhongShader();
    Mesh* cube = Mesh::GetCube(phong);
    Texture* texture = Utilities::ImportTexture(GetAbsoluteResourcesPath("\\texture\\water.jpg"), GL_TEXTURE_2D);

    SceneObject *cubeObject = new SceneObject();
    cubeObject->Name = "Cube";
    cubeObject->transform.rotation.y = 45.0f;
    auto cubemp = cubeObject->AddComponent<MeshComponent>();
    cubemp->SetMesh(cube);
    cubemp->SetMaterial(new Material(phong));
    

    SceneObject *obj = new SceneObject();
    obj->Name = "Wave Plane";
    auto mp = obj->AddComponent<MeshComponent>();
    auto wm = obj->AddComponent<WaveManagerComponent>();
    
    wm->AddFloatingObject(cubeObject);

    wm->AddCircularWave(glm::vec3(100.0f), 1.0f, 100.0f, 20.0f);
    wm->AddCircularWave(glm::vec3(100.0f, 0.0f, 100.0f), 1.0f, 100.0f, 20.0f);
    wm->AddCircularWave(glm::vec3(100.0f, 0.0f, -1000.0f), 1.0f, 100.0f, 20.0f);
    wm->AddDirectionalWave(25.0f, 1.0f, 100.0f, 20.0f);
    wm->AddDirectionalWave(115.0f, 1.0f, 100.0f, 20.0f);
    wm->AddDirectionalWave(205.0f, 1.0f, 100.0f, 20.0f);
    
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

    scene.AddSceneObject(cubeObject);
    scene.AddSceneObject(obj);
    scene.AddSceneObject(cameraObject);
    scene.AddSceneObject(light);
    renderer->SetScene(&scene);
    renderer->RenderLoop();
    return 0;
}