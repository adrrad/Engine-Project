#include "renderer/Renderer.hpp"

#include <glad/glad.h>


#include <iostream>

namespace Rendering
{

void Renderer::Initialise()
{
    _windowManager = WindowManager::GetInstance();
    _activeWindow = _windowManager->CreateWindow("Lels", 100, 100);
    
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) throw std::exception("Failed to initialize GLAD");
}

Renderer::Renderer()
{
    Initialise();
}

void Renderer::SetScene(Scene* scene)
{
    _scene = scene;
}

void Renderer::Render()
{
    if(_scene == nullptr) throw std::exception("Renderer::Render: _scene is nullptr!");
    for(SceneObject& object : *_scene->GetSceneObjects())
    {
        Mesh* mesh = object._mesh;
        glBindVertexArray(mesh->GetVAO());
        //glDrawElements(GL_TRIANGLES, mesh->GetIndexCount(), GL_UNSIGNED_INT, 0);
    }
}

void Renderer::RenderLoop()
{
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    while(!_windowManager->WindowShouldClose(_activeWindow))
    {
        glClear(GL_COLOR_BUFFER_BIT);
        _windowManager->SwapBuffers(_activeWindow);
        _windowManager->PollEvents();
    }
}

} // namespace Rendering
