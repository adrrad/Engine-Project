#pragma once

#include "rendering/Renderqueue.hpp"
#include "rendering/Renderer.hpp"
#include "core/Scene.hpp"


namespace Engine::HLRendering
{
    
class RenderpassStrategy
{
protected:

    Rendering::Renderer* m_renderer;

public:

    inline RenderpassStrategy(Rendering::Renderer* renderer)
    {
        m_renderer = renderer;
    }

    virtual ~RenderpassStrategy() {}

    /**
     * @brief Records a renderpass used to draw the scene.
     * 
     * @param scene Pointer to the active scene.
     */
    virtual void RecordRenderpass(Core::Scene* scene) = 0;
    
    /**
     * @brief Returns a renderqueue with rendering instructions.
     * 
     * @return Rendering::Renderqueue* A pointer to a renderqueue object.
     */
    virtual Rendering::Renderqueue* GetRenderqueue() = 0;

};

} // namespace Engine::HLRendering
