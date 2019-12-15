#include "renderer/Renderer.hpp"
#include "renderer/Mesh.hpp"
#include "renderer/Scene.hpp"

#include "components/CameraComponent.hpp"
#include "components/MovementComponent.hpp"
#include "components/DirectionalLightComponent.hpp"
#include "components/MeshComponent.hpp"
#include "components/WaveManagerComponent.hpp"

#include "utilities/Utilities.hpp"

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
    Mesh *m = Mesh::GetParticlePlane(400, 400, shader, 100.0f); //new Mesh(vertices, indices, shader);
    //shader->SetVec3("u_waveCenter", glm::vec3(5.0f, -1000.0f, 1.0f));
    renderer->GetGLErrors();
    SceneObject *obj = new SceneObject();
    auto mp = obj->AddComponent<MeshComponent>();
    auto wm = obj->AddComponent<WaveManagerComponent>();
    
    wm->AddCircularWave(glm::vec3(0.0f), 1.0f, 100.0f, 20.0f);
    wm->AddDirectionalWave(25.0f, 1.0f, 100.0f, 20.0f);
    mp->SetMesh(m);
    Material* mat = new Material(shader);
    mp->SetMaterial(mat);
    wm->SetGerstnerMaterial(mat);
    obj->transform.position = glm::vec3(0.0f, -0.5f, 0.0f);
    //obj->transform.rotation = glm::vec3(90.0f, 0.0f, 0.0f);
    //obj->transform.scale = glm::vec3(10.0f, 10.0f, 1.0f);
    SceneObject* cameraObject = new SceneObject();
    cameraObject->transform.position = glm::vec3(0, 5, 0);
    auto cam = cameraObject->AddComponent<CameraComponent>();
    cam->SetMain();
    auto mov = cameraObject->AddComponent<MovementComponent>();

    SceneObject* light = new SceneObject();
    light->transform.position.y = 5.0f;
    light->transform.rotation.x = 45.0f;
    auto lcomp = light->AddComponent<DirectionalLightComponent>();
    lcomp->SetDebugDrawDirectionEnabled();

    scene.AddSceneObject(obj);
    scene.AddSceneObject(cameraObject);
    scene.AddSceneObject(light);
    renderer->SetScene(&scene);
    renderer->RenderLoop();
    return 0;
}