#pragma once


#include "renderer/WindowManager.hpp"
#include "renderer/Framebuffer.hpp"
#include "renderer/Renderpass.hpp"
#include "renderer/Shader.hpp"
#include "renderer/Mesh.hpp"
#include "renderer/Scene.hpp"
#include "renderer/Skybox.hpp"

#include "components/MeshComponent.hpp"


namespace Rendering
{

class Renderer
{
    const uint32_t _windowWidth = 1600;
    const uint32_t _windowHeight = 1024;
private:
    static Renderer* _instance;
    WindowManager* _windowManager;
    Scene* _scene = nullptr;
    uint32_t _activeWindow;
    float _totalTime = 0;
    Camera *_mainCamera = nullptr;
    std::vector<PointLight*> _pointLights;
    PointLight* _pointLight = nullptr;
    DirectionalLight *_directionalLight = nullptr;
    std::vector <LineSegment> _lineSegments;

    Skybox* _skybox = nullptr;

    Shader* _lineShader = nullptr;
    uint32_t _lineVAO = 0, _lineVBO = 0;
    uint32_t _currentLineVertexCount = 0;
    const uint32_t _maxLineVertexCount = 100;

    bool _hdr = false, bloom = false;
    uint32_t _hdrFBO;
    uint32_t _colorBuffer;
    uint32_t _rboDepth;
    Framebuffer* _hdrFramebuffer;
    Framebuffer* _pingpongbuffers[2];
    Shader *_hdrShader, *_blurShader;
    Material* _hdrMat;
    Mesh* _hdrQuad;

    Renderpass* _rp = nullptr;

    float exposure = 1.0f;

    void CreateLineBuffer(uint32_t byteSize);
    void CreateRGBA16fFramebuffer();
    void Initialise();
    void InitialiseImGUI();
    void SetupDebugCallback();
    void Render();
    void RenderSceneInspector();
    void RenderGUI();
    void RenderLine(LineSegment& line, uint32_t offset);
    void ResetFrameData();
    

    Renderer();

public:
    void UpdateUniforms(Components::MeshComponent* comp);
    
    static Renderer* GetInstance();

    void SetScene(Scene* scene);
    
    void RenderLoop();

    void SetMainCamera(Camera* camera);

    PointLight* GetNewPointLight();

    void SetDirectionalLight(DirectionalLight* directionalLight);

    float GetAspectRatio();
    
    void GetGLErrors();

    void DrawLineSegment(LineSegment segment);

    void SetSkybox(Skybox* skybox);

};

} // namespace Rendering
