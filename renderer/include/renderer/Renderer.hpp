#pragma once


#include "renderer/WindowManager.hpp"
#include "renderer/Scene.hpp"
#include "renderer/Skybox.hpp"
#include "components/MeshComponent.hpp"
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
    float _totalTime = 0;
    Camera *_mainCamera = nullptr;
    DirectionalLight *_directionalLight = nullptr;
    std::vector <LineSegment> _lineSegments;

    Skybox* _skybox = nullptr;

    Shader* _lineShader = nullptr;
    uint32_t _lineVAO = 0, _lineVBO = 0;
    uint32_t _currentLineVertexCount = 0;
    const uint32_t _maxLineVertexCount = 100;

    void CreateLineBuffer(uint32_t byteSize);
    void Initialise();
    void InitialiseImGUI();
    void SetupDebugCallback();
    void Render();
    void RenderSceneInspector();
    void RenderGUI();
    void RenderLine(LineSegment& line, uint32_t offset);
    void ResetFrameData();
    void UpdateUniforms(Components::MeshComponent* comp);

    Renderer();

public:
    static Renderer* GetInstance();

    void SetScene(Scene* scene);
    
    void RenderLoop();

    void SetMainCamera(Camera* camera);

    void SetDirectionalLight(DirectionalLight* directionalLight);

    float GetAspectRatio();
    
    void GetGLErrors();

    void DrawLineSegment(LineSegment segment);

    void SetSkybox(Skybox* skybox);

};

} // namespace Rendering
