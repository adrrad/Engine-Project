#pragma once

#include "rpstrategies/RenderpassStrategy.hpp"

#include "EngineComponent.hpp"
#include "gameplay/SceneManager.hpp"
#include "rendering/Renderer.hpp"
#include "rendering/RVM.hpp"
#include "memory/Allocator.hpp"

namespace Rendering
{
    
class HLRenderer : public EngineSubsystem<HLRenderer>
{
friend class EngineSubsystem<HLRenderer>;
private:

    Renderer* m_renderer;

    RenderpassStrategy* m_rpStrategy;

    RVM* m_renderingVM;

    /**
     * @brief Set the renderpass strategy to the default strategy (deferred strategy).
     * 
     */
    void SetDefaultRPStrategy();

    /**
     * @brief Construct a new HLRenderer object.
     * 
     */
    HLRenderer();

    void InitialiseResources();

public:

    void Initialise() override;

    /**
     * @brief Draws a frame to the backbuffer based on the active scene and rendering strategy.
     * 
     */
    void DrawFrame();

    /**
     * @brief Sets the active rendering strategy.
     * 
     * @tparam T The type of renderpass strategy to use.
     */
    template<typename T>
    void SetRPStrategy();

};


template<typename T>
void HLRenderer::SetRPStrategy()
{
    m_rpStrategy = New<T>(m_renderer);
}

} // namespace Rendering
