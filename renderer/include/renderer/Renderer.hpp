#pragma once


#include "renderer/WindowManager.hpp"
#include "renderer/Scene.hpp"

namespace Rendering
{

class Renderer
{

private:
    WindowManager* _windowManager;
    Scene* _scene = nullptr;
    uint32_t _activeWindow;
    
    void Initialise();
    void Render();
    
public:
    Renderer();
    void SetScene(Scene* scene);
    
    void RenderLoop();
};

} // namespace Rendering
