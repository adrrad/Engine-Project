#include "DeferredShadingStrategy.hpp"

#include "rendering/HLRenderer.hpp"
#include "gameplay/ComponentManager.hpp"
#include "components/LightComponent.hpp"
#include "components/CameraComponent.hpp"
#include "platform/WindowManager.hpp"
#include "gameplay/ecs/systems/ECSRenderingSystem.hpp"
#include "gameplay/ecs/ECSSystemManager.hpp"

using namespace Rendering;

namespace Rendering
{


void DeferredShadingStrategy::InitialiseResources()
{
    glm::vec2 dims = m_renderer->GetWindowDimensions();
    m_gbuffer = Framebuffer::Create("gBuffer", u32(dims.x), u32(dims.y))
        .WithColorbuffer("position", GL_RGBA16F)
        .WithColorbuffer("normal", GL_RGBA16F)
        .WithColorbuffer("reflectance", GL_RGBA16F)
        .WithColorbuffer("albedoSpec", GL_RGBA)
        .WithDepthbuffer("depth")
        .WithDepthRenderbuffer()
        .Build();
    Platform::WindowManager::GetInstance()->RegisterWindowResizeCallback(
        [&](int w, int h)
        {
            m_gbuffer->Rebuild(w,h);
        }
    );
}

void DeferredShadingStrategy::Geometrypass(Renderpass::RenderpassBuilder& rpb, Gameplay::Scene* scene)
{
    auto camcomp = Gameplay::CameraComponent::GetMainCamera();
    Geometry::Volume* frustum = camcomp->GetViewVolume();
    scene->GetDynamicTree()->Rebuild();
    rpb.NewSubpass("Geometry", SubpassFlags::DEFAULT, 1000)
        .UseFramebuffer(m_gbuffer)
        .UseCamera(Gameplay::ComponentManager::GetComponentPool<Gameplay::CameraComponent>()->GetComponents()[0]);
    // Record static and dynamic objects
    scene->GetStaticTree()->ForEach(frustum, [&](Gameplay::MeshComponent* m) { rpb.DrawMesh(m); } );
    scene->GetDynamicTree()->ForEach(frustum, [&](Gameplay::MeshComponent* m) { rpb.DrawMesh(m); } );
    // auto renderingSystem = Gameplay::ECSSystemManager::GetInstance()->GetSystem<Gameplay::ECSRenderingSystem>();
    // renderingSystem->Render(rpb);
}

void DeferredShadingStrategy::Shadowpass(Renderpass::RenderpassBuilder& rpb, Gameplay::Scene* scene)
{
    auto lights = Gameplay::ComponentManager::GetComponentPool<Gameplay::LightComponent>()->GetComponents();
    // Shadow pass
    auto shadowpass = [&](Gameplay::MeshComponent* m) 
    { 
        rpb.BindMeshInstance(m); 
        rpb.DrawMesh(m, false);
    }; 
    rpb.NewSubpass("Shadowmapping");
    rpb.UseShader(m_renderer->GetShader("Shadowmap")->GetProgramID());
    for(auto l : lights)
    {
        if(l->GetType() == Gameplay::LightType::DIRECTIONAL)
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

void DeferredShadingStrategy::Lightingpass(Renderpass::RenderpassBuilder& rpb, Gameplay::Scene* scene)
{
    // Lighting pass
    auto lights = Gameplay::ComponentManager::GetComponentPool<Gameplay::LightComponent>()->GetComponents();
    rpb.NewSubpass("Lighting", SubpassFlags::ENABLE_BLENDING | SubpassFlags::DISABLE_DEPTHMASK);
    rpb.UseFramebuffer(Framebuffer::GetDefault());
    rpb.UseShader(m_renderer->GetShader("Light")->GetProgramID());
    rpb.BindFramebufferTextures(m_gbuffer);
    for(auto l : lights)
    {
        if(l->GetType() != Gameplay::LightType::DIRECTIONAL) continue;
        rpb.BindFramebufferTextures(l->GetShadowmap(), true);
        rpb.RenderLightPass(l);
    }
}

void DeferredShadingStrategy::GUIpass(Renderpass::RenderpassBuilder& rpb, Gameplay::Scene* scene)
{
    // GUI & overlay
    rpb.NewSubpass("Overlay", SubpassFlags::DISABLE_DEPTHMASK | SubpassFlags::ENABLE_BLENDING);
}

DeferredShadingStrategy::DeferredShadingStrategy(Renderer* renderer) : RenderpassStrategy(renderer)
{
    m_renderpass = nullptr;
    InitialiseResources();
}

void DeferredShadingStrategy::RecordRenderpass(Gameplay::Scene* scene)
{
    auto renderpassbuilder = Renderpass::Create(m_allocator);
    Geometrypass(renderpassbuilder, scene);
    Shadowpass(renderpassbuilder, scene);
    Lightingpass(renderpassbuilder, scene);
    GUIpass(renderpassbuilder, scene);
    if(m_renderpass != nullptr) Delete(m_renderpass);
    m_renderpass = renderpassbuilder.Build();
}

Renderqueue* DeferredShadingStrategy::GetRenderqueue()
{
    if(m_renderpass == nullptr) return nullptr;
    return m_renderpass->GetRenderqueue();
}

} // namespace Rendering
