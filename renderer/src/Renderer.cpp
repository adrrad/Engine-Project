#include "renderer/Renderer.hpp"

#include <glad/glad.h>


#include <iostream>

namespace Rendering
{

void Renderer::Initialise()
{
    _windowManager = WindowManager::GetInstance();
    _activeWindow = _windowManager->CreateWindow("Lels", 1024, 800);
    
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) throw std::exception("Failed to initialize GLAD");

    glDepthRange(1,-1);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS); 
    //glCullFace(GL_CW);
    GetGLErrors();
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
        mesh->GetShader()->Use();
        glBindVertexArray(mesh->GetVAO());
        glDrawElements(GL_TRIANGLES, mesh->GetIndexCount(), GL_UNSIGNED_INT, 0);
    }
}

void Renderer::RenderLoop()
{
    glClearColor(0.0f, 0.1f, 0.0f, 0.0f);
    
    while(!_windowManager->WindowShouldClose(_activeWindow))
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        Render();
        _windowManager->SwapBuffers(_activeWindow);
        _windowManager->PollEvents();
        GetGLErrors();
    }
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
