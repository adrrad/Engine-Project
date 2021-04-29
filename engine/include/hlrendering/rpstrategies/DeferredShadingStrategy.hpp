#pragma once

#include "hlrendering/RenderpassStrategy.hpp"

#include "rendering/Renderpass.hpp"

namespace Engine::HLRendering
{
    
class DeferredShadingStrategy : public RenderpassStrategy
{
private:
    Rendering::Renderpass* m_renderpass;

    Rendering::Framebuffer* m_gbuffer;

    void InitialiseResources();

    void Geometrypass(Rendering::Renderpass::RenderpassBuilder& rpb, Core::Scene* scene);

    void Shadowpass(Rendering::Renderpass::RenderpassBuilder& rpb, Core::Scene* scene);

    void Lightingpass(Rendering::Renderpass::RenderpassBuilder& rpb, Core::Scene* scene);

    void GUIpass(Rendering::Renderpass::RenderpassBuilder& rpb, Core::Scene* scene);

public:

    DeferredShadingStrategy(Rendering::Renderer* renderer);

    /**
     * @brief Records a renderpass used to draw the scene.
     * 
     * @param scene Pointer to the active scene.
     */
    void RecordRenderpass(Core::Scene* scene) override;

    /**
     * @brief Returns a renderqueue with rendering instructions.
     * 
     * @return Rendering::Renderqueue* A pointer to a renderqueue object.
     */
    Rendering::Renderqueue* GetRenderqueue() override;

};

} // namespace Engine::HLRendering
