#pragma once

#include "EngineComponent.hpp"
#include "rendering/Renderqueue.hpp"
#include "rendering/Renderer.hpp"
#include "gameplay/Scene.hpp"


namespace Rendering
{

class RenderpassStrategy : public SubsystemComponent<Renderer>
{
protected:

    Renderer* m_renderer;

public:

    inline RenderpassStrategy(Renderer* renderer)
    {
        m_renderer = renderer;
    }

    virtual ~RenderpassStrategy() {}

    /**
     * @brief Records a renderpass used to draw the scene.
     * 
     * @param scene Pointer to the active scene.
     */
    virtual void RecordRenderpass(Gameplay::Scene* scene) = 0;
    
    /**
     * @brief Returns a renderqueue with rendering instructions.
     * 
     * @return Renderqueue* A pointer to a renderqueue object.
     */
    virtual Renderqueue* GetRenderqueue() = 0;

};

} // namespace Rendering
