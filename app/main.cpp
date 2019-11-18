#include "renderer/Renderer.hpp"
#include "renderer/Mesh.hpp"
#include "renderer/Scene.hpp"

#include "components/CameraComponent.hpp"
#include "components/MovementComponent.hpp"
#include "components/DirectionalLightComponent.hpp"

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
    Shader* shader = new Shader(GetAbosoluteAppFilePath("\\resources\\vertex.vert"), GetAbosoluteAppFilePath("\\resources\\fragment.frag"));
    Mesh *m = Mesh::GetParticlePlane(100, 100, shader, 10.0f); //new Mesh(vertices, indices, shader);
    SceneObject *obj = new SceneObject();
    obj->mesh = m;
    obj->transform.position = glm::vec3(0.0f, -0.5f, 0.0f);
    obj->transform.rotation = glm::vec3(90.0f, 0.0f, 0.0f);
    //obj->transform.scale = glm::vec3(10.0f, 10.0f, 1.0f);
    SceneObject* cameraObject = new SceneObject();
    auto cam = cameraObject->AddComponent<CameraComponent>();
    cam->SetMain();
    auto mov = cameraObject->AddComponent<MovementComponent>();

    SceneObject* light = new SceneObject();
    light->mesh = Mesh::GetQuad(shader);
    light->transform.rotation = glm::vec3(0.0f);
    auto lcomp = light->AddComponent<DirectionalLightComponent>();

    scene.AddSceneObject(obj);
    scene.AddSceneObject(cameraObject);
    scene.AddSceneObject(light);
    renderer->SetScene(&scene);
    renderer->RenderLoop();
    return 0;
}