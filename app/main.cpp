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
    Vertex v1 = {{0.5f,  0.5f, 0.0f}, {0,0,0}, {0,0} };
    Vertex v2 = {{0.5f, -0.5f, 0.0f}, {0,0,0}, {0,0} };
    Vertex v3 = {{-0.5f, -0.5f, 0.0f}, {0,0,0}, {0,0} };
    Vertex v4 = {{-0.5f,  0.5f, 0.0f}, {0,0,0}, {0,0} };
    std::vector<Vertex> vertices = { v1, v2, v3, v4 };
    std::vector<uint32_t> indices = { 0, 1, 3, 1, 2, 3};
    Shader* shader = Shader::GetGerstnerWaveShader();
    Mesh* m = Mesh::GetParticlePlane(400, 400, shader, 100.0f); //new Mesh(vertices, indices, shader);

    Shader* phong = Shader::GetPhongShader();
    Mesh* cube = Mesh::GetCube(phong);
    Texture* texture = Utilities::ImportTexture(GetAbsoluteResourcesPath("\\texture\\aa_beauty_and_the_sun.png"), GL_TEXTURE_2D);

    SceneObject *cubeObject = new SceneObject();
    auto cubemp = cubeObject->AddComponent<MeshComponent>();
    cubemp->SetMesh(cube);
    cubemp->SetMaterial(new Material(phong));
    

    SceneObject *obj = new SceneObject();
    auto mp = obj->AddComponent<MeshComponent>();
    auto wm = obj->AddComponent<WaveManagerComponent>();
    
    wm->AddCircularWave(glm::vec3(0.0f), 1.0f, 100.0f, 20.0f);
    wm->AddDirectionalWave(25.0f, 1.0f, 100.0f, 20.0f);
    mp->SetMesh(m);
    Material* mat = new Material(shader);
    mp->SetMaterial(mat);
    mp->SetTexture(texture);
    wm->SetGerstnerMaterial(mat);
    obj->transform.position = glm::vec3(0.0f, -0.5f, 0.0f);

    
    // Texture* back = Utilities::ImportTexture(GetAbsoluteResourcesPath("\\texture\\sor_sea\\sea_bk.JPG"));
    // Texture* front = Utilities::ImportTexture(GetAbsoluteResourcesPath("\\texture\\sor_sea\\sea_ft.JPG"));
    // Texture* left = Utilities::ImportTexture(GetAbsoluteResourcesPath("\\texture\\sor_sea\\sea_lf.JPG"));
    // Texture* right = Utilities::ImportTexture(GetAbsoluteResourcesPath("\\texture\\sor_sea\\sea_rt.JPG"));
    // Texture* top = Utilities::ImportTexture(GetAbsoluteResourcesPath("\\texture\\sor_sea\\sea_up.JPG"));
    // Texture* bot = Utilities::ImportTexture(GetAbsoluteResourcesPath("\\texture\\sor_sea\\sea_dn.JPG"));

    Texture* back = Utilities::ImportTexture(GetAbsoluteResourcesPath("\\texture\\skybox\\back.tga"));
    Texture* front = Utilities::ImportTexture(GetAbsoluteResourcesPath("\\texture\\skybox\\front.tga"));
    Texture* left = Utilities::ImportTexture(GetAbsoluteResourcesPath("\\texture\\skybox\\left.tga"));
    Texture* right = Utilities::ImportTexture(GetAbsoluteResourcesPath("\\texture\\skybox\\right.tga"));
    Texture* top = Utilities::ImportTexture(GetAbsoluteResourcesPath("\\texture\\skybox\\top.tga"));
    Texture* bot = Utilities::ImportTexture(GetAbsoluteResourcesPath("\\texture\\skybox\\bottom.tga"));

    SceneObject* cameraObject = new SceneObject();
    cameraObject->transform.position = glm::vec3(0, 5, 0);
    auto cam = cameraObject->AddComponent<CameraComponent>();
    cam->SetMain();
    try
    {
        auto skybox = cameraObject->AddComponent<SkyboxComponent>();
        skybox->SetTextures(right, left, top, bot, back, front);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    
    auto mov = cameraObject->AddComponent<MovementComponent>();
    mov->SetCamera(cam);
    mov->SetWaveManager(wm);
    SceneObject* light = new SceneObject();
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