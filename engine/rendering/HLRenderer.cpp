#include "HLRenderer.hpp"

#include "rpstrategies/DeferredShadingStrategy.hpp"


#include "rendering/Shader.hpp"
#include "rendering/GLSLStruct.hpp"

#include "memory/MemoryManager.hpp"
#include "Debug.hpp"

using namespace Rendering;

namespace Rendering
{

void HLRenderer::SetDefaultRPStrategy()
{
    SetRPStrategy<DeferredShadingStrategy>();
}

HLRenderer::HLRenderer()
: m_renderer(nullptr)
, m_rpStrategy(nullptr)
, m_renderingVM(nullptr)
{
    
}

void HLRenderer::InitialiseResources() 
{
    GLSLStruct::Create("PointLight")
        .WithVec4("Colour")
        .WithVec3("Position")
        .WithFloat("Radius")
        .Build();
    GLSLStruct::Create("DirectionalLight")
        .WithMat4("ViewProjection")
        .WithVec4("Colour")
        .WithVec3("Direction")
        .Build();
    GLSLStruct::Create("Camera")
        .WithMat4("View")
        .WithMat4("Projection")
        .WithVec4("ClearColour")
        .WithVec3("Position")
        .Build();
    GLSLStruct::Create("StandardGeometry")
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
    GLSLStruct::Create("Textures")
        .WithSampler2D("normal")
        .WithSampler2D("albedo")
        .WithSampler2D("roughness")
        .WithSampler2D("metallic")
        .WithSampler2D("ambient")
        .Build();
    GLSLStruct::Create("GlobalUniforms")
                        .WithVec2("viewportSize")
                        .WithFloat("time")
                        .Build(true, 0)->Allocate(1);
    GLSLStruct::Create("InstanceUniforms")
        .WithMat4("Model")
        .WithMat4("ViewModel")
        .WithMat4("InvT")
        .WithMat4("MVP")
        .Build(true, 1);
    GLSLStruct::Create("PBRProperties")
        .WithBool("hasNormal")
        .WithVec3("F0")
        .WithBool("hasAO")
        .Build(true, 2);
    GLSLStruct::Create("PLight").WithStruct(GLSLStruct::Get("PointLight"), "pointLight").Build(true, 3);
    GLSLStruct::Create("DLight").WithStruct(GLSLStruct::Get("DirectionalLight"), "directionalLight").Build(true, 4);
    GLSLStruct::Create("CameraBuffer").WithStruct(GLSLStruct::Get("Camera"), "camera").Build(true, 6)->Allocate(100);

    Shader::Create("Light")
        .WithPPVertexFunctions()
        .WithDeferredPBRLighting()
        .WithStruct(GLSLStruct::Get("PointLight"))
        .WithStruct(GLSLStruct::Get("DirectionalLight"))
        .WithStruct(GLSLStruct::Get("Camera"))
        .WithStruct(GLSLStruct::Get("StandardGeometry"))
        .WithUniformBlock(GLSLStruct::Get("CameraBuffer"), "", true)
        .WithUniformBlock(GLSLStruct::Get("PLight"), "")
        .WithUniformBlock(GLSLStruct::Get("DLight"), "")
        .WithUniformBlock(GLSLStruct::Get("GlobalUniforms"), "", true)
        .WithUniformStruct(
            GLSLStruct::Create("Shadowmap")
            .WithSampler2D("depth")
            .Build(),
            "shadowmap", true)
        .Build()->AllocateBuffers(100);
    Shader::Create("Deferred")
        .WithWorldSpaceVertexFunctions()
        .WithGBuffer()
        .WithStruct(GLSLStruct::Get("PointLight"))
        .WithStruct(GLSLStruct::Get("DirectionalLight"))
        .WithStruct(GLSLStruct::Get("Camera"))
        .WithStruct(GLSLStruct::Get("StandardGeometry"))
        .WithStruct(GLSLStruct::Get("Textures"))
        .WithUniformStruct(GLSLStruct::Get("Textures"), "textures", false)
        .WithUniformBlock(GLSLStruct::Get("PBRProperties"), "PBR")
        .WithUniformBlock(GLSLStruct::Get("InstanceUniforms"), "")
        .WithUniformBlock(GLSLStruct::Get("GlobalUniforms"), "", true)
        .WithUniformBlock(GLSLStruct::Get("CameraBuffer"), "", true)
        .Build()->AllocateBuffers(5000);
    Shader::Create("Shadowmap").Shadowmap().Build();
}

void HLRenderer::Initialise()
{
    SetAllocator(EngineSubsystem<Renderer>::GetSubsystemAllocator());
    m_renderer = Renderer::GetInstance();
    InitialiseResources();
    SetDefaultRPStrategy();
}

void HLRenderer::DrawFrame()
{
    Assert(m_renderer != nullptr, "HLRenderer - renderer is nullptr!");
    RVM vm;
    m_renderer->UpdateUniformBuffers();
    m_rpStrategy->RecordRenderpass(Gameplay::Scene::GetMainScene());
    Framebuffer::Clear();
    vm.Run(m_rpStrategy->GetRenderqueue());
    m_renderer->RenderLines();
    m_renderer->RenderGUI();
    glFinish();
    m_renderer->GetGLErrors();
}

    
} // namespace Rendering
