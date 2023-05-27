#pragma once

#include "../ECSSystembase.hpp"
#include "rendering/Renderpass.hpp"

namespace Gameplay
{

class ECSRenderingSystem : public ECSSystembase
{
public:
    // ECSRenderingSystem();
    // ~ECSRenderingSystem();

    void Update(float deltaTime) override;

    Signature GetSystemSignature() override;

    void Render(Rendering::Renderpass::RenderpassBuilder& rpb);

private:
    
};
  
} // namespace Gameplay
