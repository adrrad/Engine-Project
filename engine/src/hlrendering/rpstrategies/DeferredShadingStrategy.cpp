#include "hlrendering/rpstrategies/DeferredShadingStrategy.hpp"

#include "components/ComponentManager.hpp"
#include "components/LightComponent.hpp"
#include "components/CameraComponent.hpp"

using namespace Engine::Rendering;

namespace Engine::HLRendering
{

void DeferredShadingStrategy::InitialiseResources()
{
    glm::vec2 dims = m_renderer->GetWindowDimensions();
    m_gbuffer = Framebuffer::Create("gBuffer", dims.x, dims.y)
        .WithColorbuffer("position", GL_RGBA16F)
        .WithColorbuffer("normal", GL_RGBA16F)
        .WithColorbuffer("reflectance", GL_RGBA16F)
        .WithColorbuffer("albedoSpec", GL_RGBA)
        .WithDepthbuffer("depth")
        .WithDepthRenderbuffer()
        .Build();
}

void DeferredShadingStrategy::Geometrypass(Rendering::Renderpass::RenderpassBuilder& rpb, Core::Scene* scene)
{
    auto camcomp = Components::CameraComponent::GetMainCamera();
    Geometry::Volume* frustum = camcomp->GetViewVolume();
    rpb.NewSubpass("Geometry", SubpassFlags::DEFAULT, 1000)
        .UseFramebuffer(m_gbuffer)
        .UseCamera(Components::ComponentManager::GetComponentPool<Components::CameraComponent>()->GetComponents()[0]);
    // Record static and dynamic objects
    scene->GetStaticTree()->ForEach(frustum, [&](Components::MeshComponent* m) { rpb.DrawMesh(m); } );
    scene->GetDynamicTree()->ForEach(frustum, [&](Components::MeshComponent* m) { rpb.DrawMesh(m); } );
}

void DeferredShadingStrategy::Shadowpass(Rendering::Renderpass::RenderpassBuilder& rpb, Core::Scene* scene)
{
    auto lights = Components::ComponentManager::GetComponentPool<Components::LightComponent>()->GetComponents();
    // Shadow pass
    auto shadowpass = [&](Components::MeshComponent* m) 
    { 
        rpb.BindMeshInstance(m); 
        rpb.DrawMesh(m, false);
    }; 
    rpb.NewSubpass("Shadowmapping");
    rpb.UseShader(m_renderer->GetShader("Shadowmap")->GetProgramID());
    for(auto l : lights)
    {
        if(l->GetType() == Components::LightType::DIRECTIONAL)
        {
            Geometry::Frustum lightFrustum(l->ViewProjectionMatrix());
            Geometry::Volume* viewVolume = l->GetLightVolume();
            // 1. Bind the light buffer
            rpb.BindLight(l);
            // 2. Bind the shadow map in order to render to it 
            rpb.UseFramebuffer(l->GetShadowmap());
            // 3. Get each renderable gameobject's instance buffer, bind it, and render it to the shadow map
            scene->GetStaticTree()->ForEach(viewVolume, shadowpass );
            scene->GetDynamicTree()->ForEach(viewVolume, shadowpass );
            delete viewVolume;
        }
    }
}

void DeferredShadingStrategy::Lightingpass(Rendering::Renderpass::RenderpassBuilder& rpb, Core::Scene* scene)
{
    // Lighting pass
    auto lights = Components::ComponentManager::GetComponentPool<Components::LightComponent>()->GetComponents();
    rpb.NewSubpass("Lighting", SubpassFlags::ENABLE_BLENDING | SubpassFlags::DISABLE_DEPTHMASK);
    rpb.UseFramebuffer(Framebuffer::GetDefault());
    rpb.UseShader(m_renderer->GetShader("Light")->GetProgramID());
    rpb.BindFramebufferTextures(m_gbuffer);
    for(auto l : lights)
    {
        if(l->GetType() != Components::LightType::DIRECTIONAL) continue;
        rpb.BindFramebufferTextures(l->GetShadowmap(), true);
        rpb.RenderLightPass(l);
    }
}

void DeferredShadingStrategy::GUIpass(Rendering::Renderpass::RenderpassBuilder& rpb, Core::Scene* scene)
{
    // GUI & overlay
    rpb.NewSubpass("Overlay", SubpassFlags::DISABLE_DEPTHMASK | SubpassFlags::ENABLE_BLENDING);
}

DeferredShadingStrategy::DeferredShadingStrategy(Rendering::Renderer* renderer) : RenderpassStrategy(renderer)
{
    m_renderpass = nullptr;
    InitialiseResources();
}

void DeferredShadingStrategy::RecordRenderpass(Core::Scene* scene)
{
    auto renderpassbuilder = Renderpass::Create();
    Geometrypass(renderpassbuilder, scene);
    Shadowpass(renderpassbuilder, scene);
    Lightingpass(renderpassbuilder, scene);
    GUIpass(renderpassbuilder, scene);
    if(m_renderpass != nullptr) delete m_renderpass;
    m_renderpass = renderpassbuilder.Build();
}

Rendering::Renderqueue* DeferredShadingStrategy::GetRenderqueue()
{
    if(m_renderpass == nullptr) return nullptr;
    return m_renderpass->GetRenderqueue();
}

} // namespace Engine::HLRendering
