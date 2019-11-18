#include <iostream>
#include "renderer/Renderer.hpp"
#include "renderer/Mesh.hpp"
#include "renderer/Scene.hpp"

#include "components/CameraComponent.hpp"

using namespace std;

using namespace Rendering;
using namespace Components;
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
    Shader* shader = new Shader("C:\\Users\\Svampex\\Documents\\Projects\\Graphics-Programming\\app\\resources\\vertex.vert", "C:\\Users\\Svampex\\Documents\\Projects\\Graphics-Programming\\app\\resources\\fragment.frag");
    Mesh *m = new Mesh(vertices, indices, shader);
    SceneObject obj;
    obj._mesh = m;
    auto cam = obj.AddComponent<CameraComponent>();
    cam->SetMain();
    obj.transform.position = glm::vec3(0.0f, 0.0f, 1.0f);
    scene.AddSceneObject(obj);
    renderer->SetScene(&scene);
    renderer->RenderLoop();
    return 0;
}