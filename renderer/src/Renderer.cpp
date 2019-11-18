#include "renderer/Renderer.hpp"

#include <glad/glad.h>
#include <chrono>

#include <iostream>

namespace Rendering
{
Renderer* Renderer::_instance;

void Renderer::Initialise()
{
    _windowManager = WindowManager::GetInstance();
    _activeWindow = _windowManager->CreateWindow("Lels", _windowWidth, _windowHeight);
    _windowManager->SetActivewindow(_activeWindow);
    _windowManager->LockCursor(_activeWindow);
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) throw std::exception("Failed to initialize GLAD");

    glDepthRange(-1,1);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS); 
    //glCullFace(GL_CW);
    GetGLErrors();
}

Renderer::Renderer()
{
    Initialise();
}

Renderer* Renderer::GetInstance()
{
    if(_instance == nullptr) _instance = new Renderer();
    return _instance;
}

void Renderer::SetScene(Scene* scene)
{
    _scene = scene;
}

void Renderer::Render()
{
    if(_scene == nullptr) throw std::exception("Renderer::Render: _scene is nullptr!");
    for(SceneObject* object : *_scene->GetSceneObjects())
    {
        if(object->mesh == nullptr) continue;
        Mesh* mesh =  object->mesh;
        mesh->GetShader()->Use();
        UpdateUniforms(object);
        glBindVertexArray(mesh->GetVAO());
        glDrawElements(GL_TRIANGLES, mesh->GetIndexCount(), GL_UNSIGNED_INT, 0);
    }
}

void Renderer::RenderLoop()
{
    glClearColor(0.0f, 0.1f, 0.0f, 0.0f);
    
    while(!_windowManager->WindowShouldClose(_activeWindow))
    {
        auto startTime = std::chrono::high_resolution_clock::now();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        Render();
        _windowManager->SwapBuffers(_activeWindow);
        _windowManager->PollEvents();
        GetGLErrors();
        std::chrono::duration<float> endTime = std::chrono::high_resolution_clock::now() - startTime;
        float deltaTime = endTime.count();
        _totalTime += deltaTime;
        _scene->Update(deltaTime);
    }
}

void Renderer::SetMainCamera(Camera *camera)
{
    _mainCamera = camera;
}

void Renderer::SetDirectionalLight(DirectionalLight *directionalLight)
{
    _directionalLight = directionalLight;
}

void Renderer::UpdateUniforms(SceneObject *object)
{
    Mesh* mesh = object->mesh;
    Shader* shader = mesh->GetShader();
    const float* data = (const float*)_mainCamera;
    auto M = object->transform.GetModelMatrix();
    auto V = _mainCamera->ViewMatrix;
    auto P = _mainCamera->ProjectionMatrix;
    auto MVP = P * V * M;
    shader->SetMat4("camera.Projection", P, 1);
    shader->SetMat4("camera.View", V, 1);
    shader->SetMat4("mesh.Model", M, 1);
    shader->SetMat4("mesh.MVP", MVP, 1);
    shader->SetFloat("time", _totalTime);
    shader->SetVec3("dirLight.Direction", _directionalLight->Direction);
    shader->SetVec4("dirLight.Colour", _directionalLight->Colour);
}

float Renderer::GetAspectRatio()
{
    return float(_windowWidth)/float(_windowHeight);
}

const char* GetErrorMessageFromCode(uint32_t code)
{
	switch (code)
	{
	case 0x500: return "GL_INVALID_ENUM";
	case 0x501: return "GL_INVALID_VALUE";
	case 0x502: return "GL_INVALID_OPERATION";
	case 0x503: return "GL_STACK_OVERFLOW";
	case 0x504: return "GL_STACK_UNDERFLOW";
	case 0x505: return "GL_OUT_OF_MEMORY";
	case 0x506: return "GL_INVALID_FRAMEBUFFER_OPERATION";
	case 0x507: return "GL_CONTEXT_LOST";
	default:
		printf("Unknown error code %d\n", code);
	}
}

void Renderer::GetGLErrors()
{
    GLenum error = glGetError();
	while (error != GL_NO_ERROR)
	{
		const char* msg = GetErrorMessageFromCode(error);
		std::cout << error << "\n" << msg << std::endl;
		error = glGetError();
	}
}

} // namespace Rendering
