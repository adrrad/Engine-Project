#include "renderer/Renderer.hpp"
#include "components/ComponentPool.hpp"
#include "components/MeshComponent.hpp"

#include <glad/glad.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <chrono>
#include <iostream>

namespace Rendering
{
Renderer* Renderer::_instance;


void Renderer::CreateLineBuffer(uint32_t byteSize)
{
    glGenBuffers(1, &_lineVBO);
    glGenVertexArrays(1, &_lineVAO);

    glBindVertexArray(_lineVAO);
    glBindBuffer(GL_ARRAY_BUFFER, _lineVBO);
    glBufferData(GL_ARRAY_BUFFER, byteSize, nullptr, GL_DYNAMIC_DRAW);
    
    int positionAttribLocation = glGetAttribLocation(_lineShader->GetID(), "v_position");
    glEnableVertexAttribArray(positionAttribLocation);
    glVertexAttribPointer(positionAttribLocation, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (const void *)0);
    
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Renderer::Initialise()
{
    _windowManager = WindowManager::GetInstance();
    _activeWindow = _windowManager->CreateWindow("Lels", _windowWidth, _windowHeight);
    _windowManager->SetActivewindow(_activeWindow);
    _windowManager->LockCursor(_activeWindow);
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) throw std::exception("Failed to initialize GLAD");
    InitialiseImGUI();
    glDepthRange(-1,1);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glDepthFunc(GL_LESS); 
    //glCullFace(GL_CW);
    GetGLErrors();

    _lineShader = Shader::GetLineShader();
    CreateLineBuffer(_maxLineVertexCount*sizeof(glm::vec3));
}

void Renderer::InitialiseImGUI()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(_windowManager->GetGLFWWindow(_activeWindow), true);
    ImGui_ImplOpenGL3_Init("#version 330 core");
}

void Renderer::ResetFrameData()
{
    _lineSegments.clear();
    _currentLineVertexCount = 0;
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
    auto& meshComponents = Components::ComponentManager::GetComponentPool<Components::MeshComponent>()->GetComponents();
    for(auto& comp : meshComponents)
    {
        if(comp._mesh == nullptr) throw std::exception("A mesh component must have a mesh attached before rendering!");
        SceneObject* object = comp.sceneObject;
        Mesh* mesh =  comp._mesh;
        mesh->GetShader()->Use();
        UpdateUniforms(object);
        glBindVertexArray(mesh->GetVAO());
        glDrawElements(GL_TRIANGLES, mesh->GetIndexCount(), GL_UNSIGNED_INT, 0);
    }
    _lineShader->Use();
    uint32_t vertexIndex = 0;
    for(LineSegment& line : _lineSegments)
    {
        RenderLine(line, vertexIndex);
        vertexIndex += line.Vertices.size();
    }
    ResetFrameData();
}

void Renderer::RenderGUI()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    Components::ComponentManager::DrawGUIAllComponents();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Renderer::RenderLine(LineSegment& line, uint32_t offset)
{
    glm::mat4 mvp = _mainCamera->ProjectionMatrix * _mainCamera->ViewMatrix * line.Transformation;
    _lineShader->SetVec4("u_colour", line.Colour);
    _lineShader->SetMat4("u_MVP", mvp, 1);
    glLineWidth(line.Width);
    glBindVertexArray(_lineVAO);
    glDrawArrays(GL_LINE_STRIP, offset, GLsizei(line.Vertices.size()));
}

void Renderer::RenderLoop()
{
    
    
    while(!_windowManager->WindowShouldClose(_activeWindow))
    {
        glm::vec4 col = _mainCamera->BackgroundColour;
        glClearColor(col.r, col.g, col.b, col.a);
        auto startTime = std::chrono::high_resolution_clock::now();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        Render();
        RenderGUI();
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
    auto comp = object->GetComponent<Components::MeshComponent>();
    Shader* shader = comp->_mesh->GetShader();
    const float* data = (const float*)_mainCamera;
    auto M = comp->sceneObject->transform.GetModelMatrix();
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

void Renderer::DrawLineSegment(LineSegment segment)
{
    if(_currentLineVertexCount + segment.Vertices.size() <= _maxLineVertexCount)
    {
        uint32_t allocationSize = segment.Vertices.size()*sizeof(glm::vec3);
        uint32_t offset = _currentLineVertexCount*sizeof(glm::vec3);
        glBindVertexArray(_lineVAO);
        glBindBuffer(GL_ARRAY_BUFFER, _lineVBO);
        glBufferSubData(GL_ARRAY_BUFFER, offset, allocationSize, segment.Vertices.data());
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        _currentLineVertexCount += segment.Vertices.size();
        _lineSegments.push_back(segment);
    }
    else
    {
        throw std::exception("Could not draw more lines this frame!");
    }

    
}

} // namespace Rendering
