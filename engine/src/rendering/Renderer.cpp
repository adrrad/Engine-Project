#include "rendering/Renderer.hpp"
#include "components/ComponentManager.hpp"
#include "components/MeshComponent.hpp"
#include "components/LightComponent.hpp"
#include "components/CameraComponent.hpp"
#include "rendering/Debugging.hpp"
#include "utilities/Utilities.hpp"


#include "assets/resources/MeshAsset.hpp"
#include "assets/resources/ImageAsset.hpp"
#include "assets/resources/CubemapAsset.hpp"

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
Renderer* Renderer::m_instance;


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
    GLSLStruct* light = GLSLStruct::Create("Light").WithStruct(plight, "pointLight").Build(true, 3);
    m_uniformStructs["PointLight"] = plight;
    m_uniformStructs["DirectionalLight"] = dlight;
    m_uniformStructs["Camera"] = camera;
    m_uniformStructs["StandardShadingProperties"] = props;
    m_uniformStructs["PBRProperties"] = pbr;
    m_uniformStructs["Light"] = light;
    m_uniformStructs["InstanceUniforms"] = instance;
    m_uniformStructs["GlobalUniforms"] = globals;
    m_uniformStructs["Textures"] = textures;

    m_uLights = light;
    m_uData = globals;
    m_uData->Allocate(1);
}


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
    m_windowManager = Platform::WindowManager::GetInstance();
    m_activeWindow = m_windowManager->GetActiveWindow();
    m_windowManager->RegisterWindowResizeCallback(
        [&](int w, int h){
            m_windowWidth = w;
            m_windowHeight = h;
            RecreateFramebuffers();
            InvalidateRenderpass();
        });
    int gladLoaded = gladLoadGL();
    if(!gladLoaded) throw std::exception("Failed to initialize GLAD");
    SetupDebugCallback();
    InitialiseImGUI();
    CreateUniformBuffer();
    CreateFramebuffers();
    InitialiseDeferredShading();
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

void Renderer::InitialiseDeferredShading()
{
    m_targetQuad = Mesh::GetQuad();
    m_lightShader = Shader::Create("Light").WithPPVertexFunctions().WithDeferredPBRLighting().Build();
    m_lightShader->AllocateBuffers(100);
    m_lightMaterial = m_lightShader->CreateMaterial();
    m_lightMaterial->SetTexture("gBuffer.position", m_gBuffer->GetColorbuffer("position"));
    m_lightMaterial->SetTexture("gBuffer.normal", m_gBuffer->GetColorbuffer("normal"));
    m_lightMaterial->SetTexture("gBuffer.reflectance", m_gBuffer->GetColorbuffer("reflectance"));
    m_lightMaterial->SetTexture("gBuffer.albedoSpec", m_gBuffer->GetColorbuffer("albedospec"));
    m_lightMaterial->SetTexture("gBuffer.depth", m_gBuffer->GetDepthBuffer("depth"));
    m_lightMC.SetMesh(m_targetQuad);
    m_lightMC.SetMaterial(m_lightMaterial);
    // SKYBOX
    m_skybox = Mesh::GetSkybox();
    m_skyboxShader = Shader::Create("Skybox").WithSkyboxVertexFunctions().WithSkybox(true).Build();
    m_skyboxShader->AllocateBuffers(1);
    m_skyboxMaterial = m_skyboxShader->CreateMaterial();
    m_skyboxMaterial->SetTexture("gBuffer.depth", m_gBuffer->GetDepthBuffer("depth"));
    m_skyboxMaterial->SetTexture("lBuffer.colour", m_lightBuffer->GetColorbuffer("colour"));
    m_skyboxMC.SetMesh(m_skybox);
    m_skyboxMC.SetMaterial(m_skyboxMaterial);
    // Deferred shader, used by the user
    Shader::Create("Deferred").WithWorldSpaceVertexFunctions().WithGBuffer().Build();
    GetShader("Deferred")->AllocateBuffers(5000);
}

void Renderer::CreateFramebuffers()
{
    m_gBuffer = Framebuffer::Create(m_windowWidth, m_windowHeight)
        .WithColorbuffer("position", GL_RGBA16F)
        .WithColorbuffer("normal", GL_RGBA16F)
        .WithColorbuffer("reflectance", GL_RGBA16F)
        .WithColorbuffer("albedospec", GL_RGBA)
        .WithDepthbuffer("depth")
        .WithDepthRenderbuffer()
        .Build();
    m_lightBuffer = Framebuffer::Create(m_windowWidth, m_windowHeight)
        .WithColorbuffer("colour", GL_RGBA)
        .Build();
}

void Renderer::RecreateFramebuffers()
{
    m_gBuffer->Rebuild(m_windowWidth, m_windowHeight);
    m_lightBuffer->Rebuild(m_windowWidth, m_windowHeight);
}

void Renderer::ResetFrameData()
{
    m_lineSegments.clear();
    m_currentLineVertexCount = 0;
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
    Texture* texture = new Texture(textureID, width, height, numChannels, imageAssetID);
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
    
    ElementCount numSubmeshes = data->Meshes.Length;
    Array<float>& positions = data->Positions;
    ElementCount numPositions = positions.Length/3;
    std::vector<Index> indices;
    std::vector<Vertex> vertices(numPositions);

    for(Index posIndex = 0; posIndex < numPositions; posIndex++)
    {
        vertices[posIndex].Position = { positions[posIndex*3], positions[posIndex*3+1], positions[posIndex*3+2] };
    }
    
    for(Index meshIndex = 0; meshIndex < numSubmeshes; meshIndex++)
    {
        MeshIndices& meshIndices = data->Meshes[meshIndex];
        ElementCount numIndices = meshIndices.PositionIndices.Length; // TODO: Change vertex definition and this
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
    Mesh* mesh = new Mesh(vertices, indices, meshAssetID);
    m_meshes.push_back(mesh);
    m_meshMapping.insert({guid, mesh});
    return mesh;
}

Renderer::Renderer()
{
    m_linedata = new Array<glm::vec3>(m_maxLineVertexCount);
}

void Renderer::InvalidateRenderpass()
{
    delete m_rp;
    m_rp = nullptr;
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
    Cubemap* cubemap = new Cubemap(imgs.Right, imgs.Left, imgs.Top, imgs.Bot, imgs.Back, imgs.Front, cubemapAssetID);
    m_cubemaps.push_back(cubemap);
    m_cubemapMapping.insert({guid, cubemap});
    return cubemap;
}

std::unordered_map<std::string, GLSLStruct*>& Renderer::GetStdUniformStructs()
{
    return m_uniformStructs;
}

Renderer* Renderer::GetInstance()
{
    if(m_instance == nullptr) 
    {
        m_instance = new Renderer();
        m_instance->Initialise();
    }
    return m_instance;
}


void Renderer::UpdateUniformBuffers()
{
    m_uData->SetMember<Camera>(0, "camera", *m_mainCamera);
    m_uData->SetMember<DirectionalLight>(0, "directionalLight", *m_directionalLight);
    auto dims = glm::vec2(m_windowWidth,m_windowHeight);
    m_uData->SetMember<glm::vec2>(0, "viewportSize", dims);
    for(Index meshCompIndex = 0; meshCompIndex < m_meshComponents.size(); meshCompIndex++)
    {
        UPDATE_CALLINFO();
        auto comp = m_meshComponents[meshCompIndex];
        Material* mat = comp->m_material;
        auto cameraPosition = m_mainCamera->Position;
        auto M = comp->GetModelMatrix();
        auto V = m_mainCamera->ViewMatrix;
        auto P = m_mainCamera->ProjectionMatrix;
        auto MVP = P * V * M;
        auto MV = V * M;
        auto ITM = glm::inverse(glm::transpose(M));
        //TODO: Cache the member pointers to avoid calculating the every frame
        mat->SetProperty<glm::mat4x4>("InstanceUniforms", "ViewModel", MV);
        mat->SetProperty<glm::mat4x4>("InstanceUniforms", "Model", M);
        mat->SetProperty<glm::mat4x4>("InstanceUniforms", "InvT", ITM);
        mat->SetProperty<glm::mat4x4>("InstanceUniforms", "MVP", MVP);
    }
    for(Shader* s : m_shaders) s->UpdateUniformBuffers();
    m_uData->UpdateUniformBuffer();
    // m_uLights->UpdateUniformBuffer();
}

void Renderer::Render()
{
    auto pool = Components::ComponentManager::GetComponentPool<Components::MeshComponent>();
    if(pool == nullptr) throw std::exception("The mesh component pool is missing!");
    auto& meshComponents = Components::ComponentManager::GetComponentPool<Components::MeshComponent>()->GetComponents();

    auto rpb = Renderpass::Create();
    m_meshComponents = meshComponents;
    UPDATE_CALLINFO();
    UpdateUniformBuffers();
    UPDATE_CALLINFO();
    // m_rp->Execute();
    m_renderpass->Execute();

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
    glm::mat4 mvp = m_mainCamera->ProjectionMatrix * m_mainCamera->ViewMatrix;
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
    ResetFrameData();
}


void Renderer::RenderGUI()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    Components::ComponentManager::DrawGUIAllComponents();
    for(auto func : m_guiDraws) func();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Renderer::RecordScene(Core::Scene* scene)
{
    auto camcomp = Components::CameraComponent::GetMainCamera();
    Geometry::Frustum& frustum = camcomp->GetViewFrustum();
    scene->GetDynamicTree()->Rebuild();
    // Geometry pass
    auto renderpassbuilder = Renderpass::Create()
        .NewSubpass("Geometry", SubpassFlags::DEFAULT, 1000)
        .UseFramebuffer(m_gBuffer);
    // Record static and dynamic objects
    scene->GetStaticTree()->RecordRenderpass(&frustum, renderpassbuilder);
    scene->GetDynamicTree()->RecordRenderpass(&frustum, renderpassbuilder);
    // Lighting pass
    renderpassbuilder.NewSubpass("Lighting", SubpassFlags::ENABLE_BLENDING | SubpassFlags::DISABLE_DEPTHMASK); // <----------- this
    renderpassbuilder.UseFramebuffer(Framebuffer::GetDefault());
    renderpassbuilder.UseShader(GetShader("Light")->ID);
    auto vao = m_lightMC.m_material->GetVAO();
    ActiveTextureID activeTexture = GL_TEXTURE0;
    for(auto pair : m_lightMC.m_material->m_textures)
    {
        std::string name = pair.first;
        Texture* texture = pair.second;
        int uniformLocation = m_lightMC.m_material->m_shader->ULoc(name);
        UPDATE_CALLINFO2("Uniform name: " + name);
        renderpassbuilder.BindTexture(uniformLocation, activeTexture, texture->GetID(), texture->GetType());
        activeTexture+=1;
    }
    for(uint32_t lightIndex = 0; lightIndex < m_pointLights.size(); lightIndex++)
    {
        auto offset = m_uLights->GetInstanceOffset(lightIndex);
        renderpassbuilder.BindBufferRange(m_uLights->BindingIndex, m_uLights->GetUniformBuffer(), offset, m_uLights->Size);
        renderpassbuilder.DrawMesh(vao, GL_TRIANGLES, 6);
    }
    // GUI & overlay
    renderpassbuilder.NewSubpass("Overlay", SubpassFlags::DISABLE_DEPTHMASK | SubpassFlags::ENABLE_BLENDING);
    if(m_renderpass != nullptr) delete m_renderpass;
    m_renderpass = renderpassbuilder.Build();
}

void Renderer::RenderFrame()
{
    delete m_rp;
    // m_rp = m_createRPCallback();
    glm::vec4 col = m_mainCamera->BackgroundColour;
    glClearColor(col.r, col.g, col.b, col.a);
    m_uData->UpdateUniformBuffer();
    Framebuffer::Clear();
    Render();
    RenderGUI();
    glFinish();
    GetGLErrors();
}


void Renderer::SetMainCamera(Camera *camera)
{
    m_mainCamera = camera;
    m_uData->SetMember<Camera>(0, "camera", *camera);
}

void Renderer::SetRenderpass(Renderpass* rp)
{
    m_rp = rp;
}

void Renderer::SetRenderpassReconstructionCallback(std::function<Renderpass*()> func)
{
    m_createRPCallback = func;
}

PointLight* Renderer::GetNewPointLight()
{
    if(m_uLights->GetInstancesCount() == 0) m_uLights->Allocate(100);
    int index = int(m_pointLights.size());
    PointLight* p = m_uLights->GetMember<PointLight>(index, "pointLight");//new PointLight();
    index++;
    m_uData->SetMember<int>(0, "pointLightCount", index);
    m_uData->UpdateUniformBuffer();
    p->Radius = 10.0f;
    p->Position = {0.0f, 0.0f, 0.0f};
    p->Colour = {1.0f, 1.0f, 1.0f, 1.0f};
    m_pointLights.push_back(p);
    return p;
}

void Renderer::SetDirectionalLight(DirectionalLight *directionalLight)
{
    m_directionalLight = directionalLight;
    m_uData->SetMember<DirectionalLight>(0, "directionalLight", *directionalLight);
}

void Renderer::UpdateUniforms(Components::MeshComponent *comp)
{
    m_uData->SetMember<float>(0, "time", m_totalTime);
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

} // namespace Engine::Rendering
