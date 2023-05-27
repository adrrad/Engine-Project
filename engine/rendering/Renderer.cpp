#include "rendering/Renderer.hpp"
#include "gameplay/ComponentManager.hpp"
#include "components/MeshComponent.hpp"
#include "components/LightComponent.hpp"
#include "components/CameraComponent.hpp"
#include "rendering/Debugging.hpp"
#include "utilities/Utilities.hpp"
#include "memory/PoolAllocator.hpp"
#include "memory/MemoryManager.hpp"
#include "assets/resources/MeshAsset.hpp"
#include "assets/resources/ImageAsset.hpp"
#include "assets/resources/CubemapAsset.hpp"

#include <glad/glad.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <chrono>
#include <iostream>


#include "rendering/HLRenderer.hpp"

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
    Rendering::Renderer::GetInstance()->GetGLErrors();
}

namespace Rendering
{



void Renderer::CreateLineBuffer(uint32_t byteSize)
{
    UPDATE_CALLINFO();
    glGenBuffers(1, &m_lineVBO);
    glGenVertexArrays(1, &m_lineVAO);

    glBindVertexArray(m_lineVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_lineVBO);
    glBufferData(GL_ARRAY_BUFFER, byteSize, nullptr, GL_DYNAMIC_DRAW);
    
    int positionAttribLocation = glGetAttribLocation(m_lineShader->GetProgramID(), "v_position");
    glEnableVertexAttribArray(positionAttribLocation);
    glVertexAttribPointer(positionAttribLocation, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (const void *)0);
    
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}


void Renderer::Initialise()
{
    u64 memory = Core::EngineSettings::Get().Memory.RendererMemory;
    SetAllocator(Memory::MemoryManager::GetInstance()->CreateAllocator<Renderer>(memory));
    m_linedata = New<Array<glm::vec3>>(m_maxLineVertexCount);
    m_windowManager = Platform::WindowManager::GetInstance();
    m_activeWindow = m_windowManager->GetActiveWindow();
    int gladLoaded = gladLoadGL();
    if(!gladLoaded) throw std::exception("Failed to initialize GLAD");
    SetupDebugCallback();
    InitialiseImGUI();
    UPDATE_CALLINFO();
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glDepthFunc(GL_LESS); 
    GetGLErrors();
    m_lineShader = Shader::GetLineShader();
    CreateLineBuffer(m_maxLineVertexCount*sizeof(glm::vec3));

}

void Renderer::InitialiseImGUI()
{
    UPDATE_CALLINFO();
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(m_windowManager->GetGLFWWindow(m_activeWindow), true);
    ImGui_ImplOpenGL3_Init("#version 430 core");
}

void Renderer::SetupDebugCallback()
{
    GLint flags = 0;
    UPDATE_CALLINFO();
    glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    if(flags & GL_CONTEXT_FLAG_DEBUG_BIT)
    {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS); 
        glDebugMessageCallback((glDebugOutput), nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
    }
}


Texture* Renderer::CreateTexture(AssetID imageAssetID)
{
    std::string guid = imageAssetID.ToString();
    Assets::ImageAsset* asset = Assets::AssetManager::GetInstance()->GetAsset<Assets::ImageAsset>(imageAssetID);
    auto imageData = asset->GetImageData();
    uint32_t textureID = 0;
    int width = imageData->Width;
    int height = imageData->Height;
    int numChannels = imageData->NumChannels;
    int format = GL_DEPTH_COMPONENT;
    if(numChannels == 3) format = GL_RGB;
    if(numChannels == 4) format = GL_RGBA;

    UPDATE_CALLINFO();
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    UPDATE_CALLINFO();
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        format,
        width,
        height,
        0,
        format,
        GL_UNSIGNED_BYTE,
        imageData->Pixels.Data());
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    Texture* texture = New<Texture>(textureID, width, height, numChannels, imageAssetID);
    m_textures.push_back(texture);
    m_textureMapping.insert({guid, texture});
    return texture;
}

Mesh* Renderer::CreateMesh(AssetID meshAssetID)
{
    std::string guid = meshAssetID.ToString();
    Assets::MeshAsset* asset = Assets::AssetManager::GetInstance()->GetAsset<Assets::MeshAsset>(meshAssetID);
    auto data = asset->GetMeshData();
    int vbo = 0, ebo = 0;
    
    ElementCount numSubmeshes = data->Meshes.size();
    std::vector<float>& positions = data->Positions;
    ElementCount numPositions = positions.size()/3;
    std::vector<Index> indices;
    std::vector<Vertex> vertices(numPositions);

    for(Index posIndex = 0; posIndex < numPositions; posIndex++)
    {
        vertices[posIndex].Position = { positions[posIndex*3], positions[posIndex*3+1], positions[posIndex*3+2] };
    }
    
    for(Index meshIndex = 0; meshIndex < numSubmeshes; meshIndex++)
    {
        MeshIndices& meshIndices = data->Meshes[meshIndex];
        ElementCount numIndices = meshIndices.PositionIndices.size(); // TODO: Change vertex definition and this
        ElementCount indexOffset = indices.size();
        indices.resize(indexOffset + numIndices);
        for(Index i = 0; i < numIndices; i++)
        {
            Index pi = meshIndices.PositionIndices[i];
            Index ni = meshIndices.NormalIndices[i];
            Index uvi = meshIndices.UVIndices[i];
            indices[indexOffset + i] = pi;
            vertices[pi].Normal = { data->Normals[ni*3], data->Normals[ni*3+1], data->Normals[ni*3+2] };
            vertices[pi].UV = { data->UVs[uvi*2], data->UVs[uvi*2+1] };
        }
    }
    Mesh* mesh = New<Mesh>(vertices, indices, meshAssetID);
    m_meshes.push_back(mesh);
    m_meshMapping.insert({guid, mesh});
    return mesh;
}

Renderer::Renderer()
{
    
}

void Renderer::AddShader(Shader* s)
{
    m_shaders.push_back(s);
}

Shader* Renderer::GetShader(std::string name)
{
    for(auto shader : m_shaders)
    {
        if(shader->m_name == name) return shader;
    }
    return nullptr;
}

Mesh* Renderer::GetMesh(AssetID meshAssetID)
{
    std::string guid = meshAssetID.ToString();
    if(m_meshMapping.contains(guid)) return m_meshMapping[guid];
    else return CreateMesh(meshAssetID);
}

Texture* Renderer::GetTexture(AssetID imageAssetID)
{
    std::string guid = imageAssetID.ToString();
    if(m_textureMapping.contains(guid)) return m_textureMapping[guid];
    else return CreateTexture(imageAssetID);
}

Cubemap* Renderer::GetCubemap(AssetID cubemapAssetID)
{
    std::string guid = cubemapAssetID.ToString();
    if(m_cubemapMapping.contains(guid)) return m_cubemapMapping[guid];
    Assets::CubemapAsset* asset = Assets::AssetManager::GetInstance()->GetAsset<Assets::CubemapAsset>(cubemapAssetID);
    auto imgs = asset->GetImages();
    Cubemap* cubemap = New<Cubemap>(imgs.Right, imgs.Left, imgs.Top, imgs.Bot, imgs.Back, imgs.Front, cubemapAssetID);
    m_cubemaps.push_back(cubemap);
    m_cubemapMapping.insert({guid, cubemap});
    return cubemap;
}

void Renderer::UpdateUniformBuffers()
{
    auto dims = glm::vec2(m_windowWidth,m_windowHeight);
    GLSLStruct::Get("GlobalUniforms")->SetMember<glm::vec2>(0, "viewportSize", dims);
    for(Shader* s : m_shaders) s->UpdateUniformBuffers();
    GLSLStruct::Get("GlobalUniforms")->UpdateUniformBuffer();
    GLSLStruct::Get("CameraBuffer")->UpdateUniformBuffer();
}

void Renderer::RenderLines()
{
    m_lineShader->Use();

    UPDATE_CALLINFO();
    glBindVertexArray(m_lineVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_lineVBO);
    glm::vec3* data = (glm::vec3*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
    memcpy(data, m_linedata->Data(), m_linedata->Size);
    glUnmapBuffer(GL_ARRAY_BUFFER);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    uint32_t vertexIndex = 0;
    auto cam = Gameplay::CameraComponent::GetMainCamera()->m_camera;
    glm::mat4 mvp = cam->ProjectionMatrix * cam->ViewMatrix;
    m_lineShader->SetMat4("r_u_MVP", mvp, 1);
    glBindVertexArray(m_lineVAO);
    
    for(LineInfo line : m_lineSegments)
    {
        UPDATE_CALLINFO();
        auto vertexCount = line.VertexCount;
        auto width = line.Width;
        auto col = line.Colour;
        glLineWidth(width);
        m_lineShader->SetVec4("r_u_colour", col);
        glDrawArrays(GL_LINE_STRIP, vertexIndex, GLsizei(vertexCount));
        vertexIndex += vertexCount;
    }
    m_lineSegments.clear();
    m_currentLineVertexCount = 0;
}


void Renderer::RenderGUI()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    Gameplay::ComponentManager::DrawGUIAllComponents();
    for(auto func : m_guiDraws) func();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

Camera* Renderer::GetNewCamera(BufferRange* buffer)
{
    int index = int(m_cameras.size());
    auto cameraBuffer = GLSLStruct::Get("CameraBuffer");
    Camera* c = cameraBuffer->GetMember<Camera>(index, "camera");
    buffer->BindingIndex = cameraBuffer->BindingIndex;
    buffer->Handle = cameraBuffer->GetUniformBuffer();
    buffer->Offset = cameraBuffer->GetInstanceOffset(index);
    buffer->Size = cameraBuffer->Size;
    m_cameras.push_back(c);
    return c;
}

PointLight* Renderer::GetNewPointLight(BufferRange* buffer)
{
    auto lightBuffer = GLSLStruct::Get("PLight");
    if(lightBuffer->GetInstancesCount() == 0) lightBuffer->Allocate(100);
    int index = int(m_pointLights.size());
    PointLight* p = lightBuffer->GetMember<PointLight>(index, "pointLight");
    buffer->BindingIndex = lightBuffer->BindingIndex;
    buffer->Handle = lightBuffer->GetUniformBuffer();
    buffer->Offset = lightBuffer->GetInstanceOffset(index);
    buffer->Size = lightBuffer->Size;
    p->Radius = 10.0f;
    p->Position = {0.0f, 0.0f, 0.0f};
    p->Colour = {1.0f, 1.0f, 1.0f, 1.0f};
    m_pointLights.push_back(p);
    return p;
}

DirectionalLight* Renderer::GetNewDirectionalLight(BufferRange* buffer)
{
    auto lightBuffer = GLSLStruct::Get("DLight");
    if(lightBuffer->GetInstancesCount() == 0) lightBuffer->Allocate(100);
    int index = int(m_dirLights.size());
    DirectionalLight* p = lightBuffer->GetMember<DirectionalLight>(index, "directionalLight");
    buffer->BindingIndex = lightBuffer->BindingIndex;
    buffer->Handle = lightBuffer->GetUniformBuffer();
    buffer->Offset = lightBuffer->GetInstanceOffset(index);
    buffer->Size = lightBuffer->Size;

    p->Direction = {0.0f, 0.0f, 1.0f};
    p->Colour = {1.0f, 1.0f, 1.0f, 1.0f};
    m_dirLights.push_back(p);
    return p;
}

void Renderer::CreateUniformBuffer(Buffer &buffer, Size size, void* data, int usage)
{
    buffer.Size = size;
    glGenBuffers(1, &buffer.Handle);
    glBindBuffer(GL_UNIFORM_BUFFER, buffer.Handle);
    glBufferData(GL_UNIFORM_BUFFER, size, data, usage);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}


void Renderer::FreeUniformBuffer(Buffer& buffer)
{
    glDeleteBuffers(1, &buffer.Handle);
    buffer = Buffer();
}

float Renderer::GetAspectRatio()
{
    return float(m_windowWidth)/float(m_windowHeight);
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
    if(m_currentLineVertexCount + segment.Vertices.size() <= m_maxLineVertexCount)
    {
        SizeBytes allocationSize = SizeBytes(segment.Vertices.size()*sizeof(glm::vec3));
        uint32_t offset = m_currentLineVertexCount;

        memcpy(m_linedata->Data()+offset, segment.Vertices.data(), allocationSize);
        m_currentLineVertexCount += uint32_t(segment.Vertices.size());
        m_lineSegments.push_back({uint32_t(segment.Vertices.size()), segment.Width, segment.Colour });
    }
    else
    {
        //TODO: Print error instead? Or handle the exception by the engine
        // throw std::exception("Could not draw more lines this frame!");
    }
}

void Renderer::RegisterGUIDraw(std::function<void()> func)
{
    m_guiDraws.push_back(func);
}

glm::vec2 Renderer::GetWindowDimensions()
{
    return { m_windowWidth, m_windowHeight };
}

} // namespace Rendering
