#include "rendering/Renderer.hpp"
#include "components/ComponentManager.hpp"
#include "components/MeshComponent.hpp"
#include "components/LightComponent.hpp"
#include "rendering/Debugging.hpp"
#include "utilities/Utilities.hpp"


#include "assets/resources/MeshAsset.hpp"
#include "assets/resources/ImageAsset.hpp"

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
    std::cout << LastAPICallInfo::GetInstance().ToString() << std::endl;
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
    Engine::Rendering::Renderer::GetInstance()->GetGLErrors();
}

namespace Engine::Rendering
{
Renderer* Renderer::_instance;


void Renderer::CreateUniformBuffer()
{
    GLSLStruct* plight = GLSLStruct::Create("PointLight")
                        .WithVec4("Colour")
                        .WithVec3("Position")
                        .WithFloat("Radius")
                        .Build();
    GLSLStruct* dlight = GLSLStruct::Create("DirectionalLight")
                        .WithVec4("Colour")
                        .WithVec3("Direction")
                        .Build();
    GLSLStruct* camera = GLSLStruct::Create("Camera")
                        .WithMat4("View")
                        .WithMat4("Projection")
                        .WithVec4("ClearColour")
                        .WithVec3("Position")
                        .Build();
    GLSLStruct* props = GLSLStruct::Create("StandardShadingProperties")
                        .WithVec4("N")
                        .WithVec4("V")
                        .WithVec4("L")
                        .WithVec4("R")
                        .WithVec4("H")
                        .WithVec2("UV")
                        .WithMat3("TBN")
                        .WithVec4("ViewSpacePosition")
                        .WithVec4("WorldSpacePosition")
                        .Build();
    GLSLStruct* textures = GLSLStruct::Create("Textures")
                        .WithSampler2D("normal")
                        .WithSampler2D("albedo")
                        .WithSampler2D("roughness")
                        .WithSampler2D("metallic")
                        .WithSampler2D("ambient")
                        .Build();
    GLSLStruct* globals = GLSLStruct::Create("GlobalUniforms")
                        .WithStructArray(plight, "pointLights", 10)
                        .WithStruct(dlight, "directionalLight")
                        .WithStruct(camera, "camera")
                        .WithVec2("viewportSize")
                        .WithInt("pointLightCount")
                        .WithFloat("time")
                        .Build(true, 0);
    GLSLStruct* instance = GLSLStruct::Create("InstanceUniforms")
                        .WithMat4("Model")
                        .WithMat4("ViewModel")
                        .WithMat4("InvT")
                        .WithMat4("MVP")
                        .Build(true, 1);
    GLSLStruct* pbr = GLSLStruct::Create("PBRProperties")
                        .WithBool("hasNormal")
                        .WithVec3("F0")
                        .WithBool("hasAO")
                        .Build(true, 2);
    _uniformStructs["PointLight"] = plight;
    _uniformStructs["DirectionalLight"] = dlight;
    _uniformStructs["Camera"] = camera;
    _uniformStructs["StandardShadingProperties"] = props;
    _uniformStructs["PBRProperties"] = pbr;
    _uniformStructs["InstanceUniforms"] = instance;
    _uniformStructs["GlobalUniforms"] = globals;
    _uniformStructs["Textures"] = textures;

    _uData = globals;
    _uData->Allocate(1);
}


void Renderer::CreateLineBuffer(uint32_t byteSize)
{
    UPDATE_CALLINFO();
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
    _windowManager = Platform::WindowManager::GetInstance();
    _activeWindow = _windowManager->GetActiveWindow();
    _windowManager->RegisterWindowResizeCallback(
        [&](int w, int h){
            _windowWidth = w;
            _windowHeight = h;
            glViewport(0,0, w, h);
            InvalidateRenderpass();
        });
    int gladLoaded = gladLoadGL();
    if(!gladLoaded) throw std::exception("Failed to initialize GLAD");
    SetupDebugCallback();
    InitialiseImGUI();
    CreateUniformBuffer();
    UPDATE_CALLINFO();
    // glDepthRange(-1,1);
    // glEnable(GL_FRAMEBUFFER_SRGB); 
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glDepthFunc(GL_LESS); 
    GetGLErrors();
    _lineShader = Shader::GetLineShader();
    CreateLineBuffer(_maxLineVertexCount*sizeof(glm::vec3));
}

void Renderer::InitialiseImGUI()
{
    UPDATE_CALLINFO();
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(_windowManager->GetGLFWWindow(_activeWindow), true);
    ImGui_ImplOpenGL3_Init("#version 430 core");
}

void Renderer::SetupDebugCallback()
{
    GLint flags = 0;
    UPDATE_CALLINFO();
    glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    // std::cout << GL_CONTEXT_FLAG_DEBUG_BIT << std::endl;
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
    _linedata = new Engine::Array<glm::vec3>(_maxLineVertexCount);
}

void Renderer::InvalidateRenderpass()
{
    delete _rp;
    _rp = nullptr;
}

void Renderer::AddShader(Shader* s)
{
    _shaders.push_back(s);
}

Mesh* Renderer::GetMesh(AssetID meshAssetID)
{
    std::string guid = meshAssetID.ToString();
    if(m_meshMapping.contains(guid)) return m_meshMapping[guid];
    Assets::MeshAsset* asset = Assets::AssetManager::GetInstance()->GetAsset<Assets::MeshAsset>(meshAssetID);
    Mesh* mesh = new Mesh(asset->GetMeshData());
    m_meshes.push_back(mesh);
    m_meshMapping.insert({guid, mesh});
    return mesh;
}

Texture* Renderer::GetTexture(AssetID imageAssetID)
{
    std::string guid = imageAssetID.ToString();
    if(m_textureMapping.contains(guid)) return m_textureMapping[guid];
    Assets::ImageAsset* asset = Assets::AssetManager::GetInstance()->GetAsset<Assets::ImageAsset>(imageAssetID);
    Texture* texture = new Texture(asset->GetImageData());
    m_textures.push_back(texture);
    m_textureMapping.insert({guid, texture});
    return texture;
}

std::unordered_map<std::string, GLSLStruct*>& Renderer::GetStdUniformStructs()
{
    return _uniformStructs;
}

Renderer* Renderer::GetInstance()
{
    if(_instance == nullptr) 
    {
        _instance = new Renderer();
        _instance->Initialise();
    }
    return _instance;
}


void Renderer::UpdateUniformBuffers()
{
    _uData->SetMember<Camera>(0, "camera", *_mainCamera);
    _uData->SetMember<DirectionalLight>(0, "directionalLight", *_directionalLight);
    _uData->SetMember<glm::vec2>(0, "viewportSize", glm::vec2(_windowWidth,_windowHeight));
    for(Index meshCompIndex = 0; meshCompIndex < _meshComponents.size(); meshCompIndex++)
    {
        UPDATE_CALLINFO();
        auto comp = _meshComponents[meshCompIndex];
        Material* mat = comp->m_material;
        auto cameraPosition = _mainCamera->Position;
        auto M = comp->GetModelMatrix();
        auto V = _mainCamera->ViewMatrix;
        auto P = _mainCamera->ProjectionMatrix;
        auto MVP = P * V * M;
        auto MV = V * M;
        //TODO: Cache the member pointers to avoid calculating the every frame
        mat->SetProperty<glm::mat4x4>("InstanceUniforms", "ViewModel", MV);
        mat->SetProperty<glm::mat4x4>("InstanceUniforms", "Model", M);
        mat->SetProperty<glm::mat4x4>("InstanceUniforms", "InvT", glm::inverse(glm::transpose(M)));
        mat->SetProperty<glm::mat4x4>("InstanceUniforms", "MVP", MVP);
    }
    for(Shader* s : _shaders) s->UpdateUniformBuffers();
    _uData->UpdateUniformBuffer();
}

void Renderer::Render()
{
    auto pool = Components::ComponentManager::GetComponentPool<Components::MeshComponent>();
    if(pool == nullptr) throw std::exception("The mesh component pool is missing!");
    auto& meshComponents = Components::ComponentManager::GetComponentPool<Components::MeshComponent>()->GetComponents();

    auto rpb = Renderpass::Create();
    _meshComponents = meshComponents;
    UPDATE_CALLINFO();
    if(_rp == nullptr) _rp = _createRPCallback();
    UPDATE_CALLINFO();
    UpdateUniformBuffers();
    UPDATE_CALLINFO();
    _rp->Execute();

    _lineShader->Use();

    UPDATE_CALLINFO();
    glBindVertexArray(_lineVAO);
    glBindBuffer(GL_ARRAY_BUFFER, _lineVBO);
    glm::vec3* data = (glm::vec3*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    memcpy(data, _linedata->Data(), _linedata->Size);
    glUnmapBuffer(GL_ARRAY_BUFFER);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    uint32_t vertexIndex = 0;
    glm::mat4 mvp = _mainCamera->ProjectionMatrix * _mainCamera->ViewMatrix;
    _lineShader->SetMat4("r_u_MVP", mvp, 1);
    glBindVertexArray(_lineVAO);
    
    for(LineInfo line : _lineSegments)
    {
        UPDATE_CALLINFO();
        auto vertexCount = line.VertexCount;
        auto width = line.Width;
        auto col = line.Colour;
        glLineWidth(width);
        _lineShader->SetVec4("r_u_colour", col);
        glDrawArrays(GL_LINE_STRIP, vertexIndex, GLsizei(vertexCount));
        vertexIndex += vertexCount;
    }
    ResetFrameData();
}


void Renderer::RenderGUI()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    Components::ComponentManager::DrawGUIAllComponents();
    for(auto func : _guiDraws) func();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Renderer::RecordScene(Core::Scene* scene)
{

}

void Renderer::RenderFrame()
{
    delete _rp;
    _rp = _createRPCallback();
    glm::vec4 col = _mainCamera->BackgroundColour;
    glClearColor(col.r, col.g, col.b, col.a);
    _uData->UpdateUniformBuffer();
    Framebuffer::Clear();
    Render();
    RenderGUI();
    glFinish();
    GetGLErrors();
}


void Renderer::SetMainCamera(Camera *camera)
{
    _mainCamera = camera;
    _uData->SetMember<Camera>(0, "camera", *camera);
}

void Renderer::SetRenderpass(Renderpass* rp)
{
    _rp = rp;
}

void Renderer::SetRenderpassReconstructionCallback(std::function<Renderpass*()> func)
{
    _createRPCallback = func;
}

PointLight* Renderer::GetNewPointLight()
{
    int index = int(_pointLights.size());
    PointLight* p = _uData->GetMember<PointLight>(0, "pointLights["+std::to_string(index)+"]");//new PointLight();
    index++;
    _uData->SetMember<int>(0, "pointLightCount", index);
    _uData->UpdateUniformBuffer();
    p->Radius = 10.0f;
    p->Position = {0.0f, 0.0f, 0.0f};
    p->Colour = {1.0f, 1.0f, 1.0f, 1.0f};
    _pointLights.push_back(p);
    return p;
}

void Renderer::SetDirectionalLight(DirectionalLight *directionalLight)
{
    _directionalLight = directionalLight;
    _uData->SetMember<DirectionalLight>(0, "directionalLight", *directionalLight);
}

void Renderer::UpdateUniforms(Components::MeshComponent *comp)
{
    _uData->SetMember<float>(0, "time", _totalTime);
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
        SizeBytes allocationSize = SizeBytes(segment.Vertices.size()*sizeof(glm::vec3));
        uint32_t offset = _currentLineVertexCount;

        memcpy(_linedata->Data()+offset, segment.Vertices.data(), allocationSize);
        _currentLineVertexCount += uint32_t(segment.Vertices.size());
        _lineSegments.push_back({uint32_t(segment.Vertices.size()), segment.Width, segment.Colour });
    }
    else
    {
        //TODO: Print error instead? Or handle the exception by the engine
        // throw std::exception("Could not draw more lines this frame!");
    }
}

void Renderer::RegisterGUIDraw(std::function<void()> func)
{
    _guiDraws.push_back(func);
}

glm::vec2 Renderer::GetWindowDimensions()
{
    return { _windowWidth, _windowHeight };
}

} // namespace Engine::Rendering
