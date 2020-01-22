#include "renderer/Renderer.hpp"
#include "components/ComponentPool.hpp"
#include "components/MeshComponent.hpp"
#include "components/WaveManagerComponent.hpp"
#include "components/SkyboxComponent.hpp"

#include <glad/glad.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <chrono>
#include <iostream>


typedef void (APIENTRY *DEBUGPROC)
            (GLenum source,
            GLenum type,
            GLuint id,
            GLenum severity,
            GLsizei length,
            const GLchar *message,
            const void *userParam);

//Function from https://learnopengl.com/In-Practice/Debugging
void APIENTRY glDebugOutput(GLenum source, GLenum type, GLuint id, GLenum severity, 
                            GLsizei length, const GLchar *message, const void *userParam)
{
    // ignore non-significant error/warning codes
    if(id == 131169 || id == 131185 || id == 131218 || id == 131204) return; 
    
    std::cout << "---------------" << std::endl;
    std::cout << "Debug message (" << id << "): " <<  message << std::endl;

    switch (source)
    {
        case GL_DEBUG_SOURCE_API:             std::cout << "Source: API"; break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   std::cout << "Source: Window System"; break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cout << "Source: Shader Compiler"; break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:     std::cout << "Source: Third Party"; break;
        case GL_DEBUG_SOURCE_APPLICATION:     std::cout << "Source: Application"; break;
        case GL_DEBUG_SOURCE_OTHER:           std::cout << "Source: Other"; break;
    } std::cout << std::endl;

    switch (type)
    {
        case GL_DEBUG_TYPE_ERROR:               std::cout << "Type: Error"; break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "Type: Deprecated Behaviour"; break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cout << "Type: Undefined Behaviour"; break; 
        case GL_DEBUG_TYPE_PORTABILITY:         std::cout << "Type: Portability"; break;
        case GL_DEBUG_TYPE_PERFORMANCE:         std::cout << "Type: Performance"; break;
        case GL_DEBUG_TYPE_MARKER:              std::cout << "Type: Marker"; break;
        case GL_DEBUG_TYPE_PUSH_GROUP:          std::cout << "Type: Push Group"; break;
        case GL_DEBUG_TYPE_POP_GROUP:           std::cout << "Type: Pop Group"; break;
        case GL_DEBUG_TYPE_OTHER:               std::cout << "Type: Other"; break;
    } std::cout << std::endl;
    
    switch (severity)
    {
        case GL_DEBUG_SEVERITY_HIGH:         std::cout << "Severity: high"; break;
        case GL_DEBUG_SEVERITY_MEDIUM:       std::cout << "Severity: medium"; break;
        case GL_DEBUG_SEVERITY_LOW:          std::cout << "Severity: low"; break;
        case GL_DEBUG_SEVERITY_NOTIFICATION: std::cout << "Severity: notification"; break;
    } std::cout << std::endl;
    std::cout << std::endl;
}

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

    if(!gladLoadGL()) throw std::exception("Failed to initialize GLAD");
    SetupDebugCallback();
    InitialiseImGUI();
    glDepthRange(-1,1);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glDepthFunc(GL_LESS); 
    // glEnable(GL_TEXTURE_2D);
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
    ImGui_ImplOpenGL3_Init("#version 430 core");
}

void Renderer::SetupDebugCallback()
{
    GLint flags = 0;
    glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    std::cout << GL_CONTEXT_FLAG_DEBUG_BIT << std::endl;
    if(flags & GL_CONTEXT_FLAG_DEBUG_BIT)
    {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS); 

        glDebugMessageCallback((glDebugOutput), nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
    }
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
    // Unsafe in case of pools don't exist (will be nullptrs)
    auto& meshComponents = Components::ComponentManager::GetComponentPool<Components::MeshComponent>()->GetComponents();
    auto& wms = Components::ComponentManager::GetComponentPool<Components::WaveManagerComponent>()->GetComponents();
    auto& skybox = Components::ComponentManager::GetComponentPool<Components::SkyboxComponent>()->GetComponents();
    if(wms.size() > 0)
    {
        auto& wm = wms[0];
        wm->UpdateUniforms();
    }

    if(skybox.size() > 0)
    {
        auto& sb = skybox[0];
        Material* mat = sb->_material;
        Shader* shader = mat->_shader;
        shader->Use();
        const float* data = (const float*)_mainCamera;
        auto V = _mainCamera->ViewMatrix;
        auto P = _mainCamera->ProjectionMatrix;
        shader->SetMat4("r_u_camera.Projection", P, 1);
        shader->SetMat4("r_u_camera.View", V, 1);
        Mesh* mesh = sb->_cube;
        glDepthMask(GL_FALSE);
        sb->_cubemap->BindTexture();
        glBindVertexArray(mesh->GetVAO());
        glDrawElements(GL_TRIANGLES, mesh->GetIndexCount(), GL_UNSIGNED_INT, 0);
        glDepthMask(GL_TRUE);
    }

    for(auto& comp : meshComponents)
    {
        if(comp->_mesh == nullptr) throw std::exception("A mesh component must have a mesh attached before rendering!");
        SceneObject* object = comp->sceneObject;
        Mesh* mesh =  comp->_mesh;
        comp->_material->_shader->Use();
        UpdateUniforms(comp);
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

void Renderer::RenderSceneInspector()
{
    ImGui::Begin("Inspector");
    int i = 0;
    for(auto object : _scene->GetSceneObjects())
    {
        ImGui::PushID(i);
        ImGui::Text(object->Name.c_str());
        ImGui::DragFloat3("Position", &object->transform.position[0], 0.1f);
        ImGui::DragFloat3("Rotation", &object->transform.rotation[0], 0.1f);
        ImGui::DragFloat3("Scale", &object->transform.scale[0], 0.1f);
        ImGui::PopID();
        i++;
    }

    ImGui::End();
}

void Renderer::RenderGUI()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    Components::ComponentManager::DrawGUIAllComponents();
    RenderSceneInspector();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Renderer::RenderLine(LineSegment& line, uint32_t offset)
{
    glm::mat4 mvp = _mainCamera->ProjectionMatrix * _mainCamera->ViewMatrix * line.Transformation;
    _lineShader->SetVec4("r_u_colour", line.Colour);
    _lineShader->SetMat4("r_u_MVP", mvp, 1);
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

void Renderer::UpdateUniforms(Components::MeshComponent *comp)
{
    Shader* shader = comp->_material->_shader;
    Material* mat = comp->_material;
    const float* data = (const float*)_mainCamera;
    auto M = comp->sceneObject->transform.GetModelMatrix();
    auto V = _mainCamera->ViewMatrix;
    auto P = _mainCamera->ProjectionMatrix;
    auto MVP = P * V * M;
    bool hasTexture = comp->_texture != nullptr;
    mat->UpdateUniforms();
    shader->SetMat4("r_u_camera.Projection", P, 1);
    shader->SetMat4("r_u_camera.View", V, 1);
    shader->SetMat4("r_u_mesh.Model", M, 1);
    shader->SetMat4("r_u_mesh.MVP", MVP, 1);
    shader->SetFloat("r_u_time", _totalTime);
    shader->SetVec3("r_u_dirLight.Direction", _directionalLight->Direction);
    shader->SetVec4("r_u_dirLight.Colour", _directionalLight->Colour);
    shader->SetInt("r_u_surface.HasTexture", hasTexture);
    if(hasTexture)
    {
        comp->_texture->BindTexture();
    }
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
