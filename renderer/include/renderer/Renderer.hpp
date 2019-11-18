#pragma once


#include "renderer/WindowManager.hpp"
#include "renderer/Scene.hpp"

namespace Rendering
{

class Renderer
{
    const uint32_t _windowWidth = 1024;
    const uint32_t _windowHeight = 800;
    
private:
    static Renderer* _instance;
    WindowManager* _windowManager;
    Scene* _scene = nullptr;
    uint32_t _activeWindow;
    
    Camera *_mainCamera = nullptr;

    void Initialise();
    void Render();
    Renderer();
public:
    static Renderer* GetInstance();

    void SetScene(Scene* scene);
    
    void RenderLoop();

    void SetMainCamera(Camera *camera);

    void UpdateUniforms(Shader *shader);

    float GetAspectRatio();
    
    void GetGLErrors();

    
};

} // namespace Rendering
