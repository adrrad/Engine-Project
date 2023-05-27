#pragma once

#include "RenderpassStrategy.hpp"

#include "rendering/Renderpass.hpp"

namespace Rendering
{
    
class DeferredShadingStrategy : public RenderpassStrategy
{
private:
    Renderpass* m_renderpass;

    Framebuffer* m_gbuffer;

    void InitialiseResources();

    void Geometrypass(Renderpass::RenderpassBuilder& rpb, Gameplay::Scene* scene);

    void Shadowpass(Renderpass::RenderpassBuilder& rpb, Gameplay::Scene* scene);

    void Lightingpass(Renderpass::RenderpassBuilder& rpb, Gameplay::Scene* scene);

    void GUIpass(Renderpass::RenderpassBuilder& rpb, Gameplay::Scene* scene);

public:

    DeferredShadingStrategy(Renderer* renderer);

    /**
     * @brief Records a renderpass used to draw the scene.
     * 
     * @param scene Pointer to the active scene.
     */
    void RecordRenderpass(Gameplay::Scene* scene) override;

    /**
     * @brief Returns a renderqueue with rendering instructions.
     * 
     * @return Renderqueue* A pointer to a renderqueue object.
     */
    Renderqueue* GetRenderqueue() override;

};

} // namespace Rendering
