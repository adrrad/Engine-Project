#pragma once

#include "RenderpassStrategy.hpp"

#include "core/SceneManager.hpp"
#include "rendering/Renderer.hpp"
#include "rendering/RVM.hpp"


namespace Engine::HLRendering
{
    
class HLRenderer
{

private:
    
    Core::SceneManager* m_sceneManager;

    Rendering::Renderer* m_renderer;

    RenderpassStrategy* m_rpStrategy;

    Rendering::RVM* m_renderingVM;

    /**
     * @brief Set the renderpass strategy to the default strategy (deferred strategy).
     * 
     */
    void SetDefaultRPStrategy();

public:

    /**
     * @brief Construct a new HLRenderer object.
     * 
     * @param renderer Pointer to the low level renderer object.
     * @param manager Pointer to the scene manager.
     */
    HLRenderer(Rendering::Renderer* renderer, Core::SceneManager* manager);

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
    m_rpStrategy = new T(m_renderer);
}

} // namespace Engine::HLRendering
