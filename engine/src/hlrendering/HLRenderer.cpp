#include "hlrendering/HLRenderer.hpp"

#include "hlrendering/rpstrategies/DeferredShadingStrategy.hpp"

namespace Engine::HLRendering
{

void HLRenderer::SetDefaultRPStrategy()
{
    SetRPStrategy<DeferredShadingStrategy>();
}

HLRenderer::HLRenderer(Rendering::Renderer* renderer, Core::SceneManager* manager)
{
    m_renderer = renderer;
    m_sceneManager = manager;
    m_renderingVM = new Rendering::RVM();
    SetDefaultRPStrategy();
}

void HLRenderer::DrawFrame()
{
    m_rpStrategy->RecordRenderpass(m_sceneManager->GetActiveScene());
    m_renderingVM->Run(m_rpStrategy->GetRenderqueue());
}
    
} // namespace Engine::HLRendering
