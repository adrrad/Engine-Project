#pragma once


#include "renderer/WindowManager.hpp"
#include "renderer/GLSLStruct.hpp"
#include "renderer/Framebuffer.hpp"
#include "renderer/Renderpass.hpp"
#include "renderer/Shader.hpp"
#include "renderer/Mesh.hpp"
#include "core/Scene.hpp"
#include "renderer/Skybox.hpp"
#include "renderer/RenderingTypedefs.hpp"
#include "renderer/RenderingStructs.hpp"
#include "renderer/GLSLStruct.hpp"

#include "components/MeshComponent.hpp"

#include <unordered_map>
#include <string>
#include <functional>

namespace Rendering
{

class Renderer
{

private:
    uint32_t _windowWidth = 1600;
    uint32_t _windowHeight = 1024;
    static Renderer* _instance;
    WindowManager* _windowManager;
    Engine::Core::Scene* _scene = nullptr;
    uint32_t _activeWindow;
    float _totalTime = 0;
    Camera *_mainCamera = nullptr;
    std::vector<PointLight*> _pointLights;
    PointLight* _pointLight = nullptr;
    DirectionalLight *_directionalLight = nullptr;
    std::vector <LineSegment> _lineSegments;
    std::vector <Shader*> _shaders;

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
    Framebuffer* _gBuffer;
    Shader *_hdrShader, *_blurShader;
    Material* _hdrMat;
    Mesh* _hdrQuad;

    Renderpass* _rp = nullptr;
    std::function<Renderpass*()> _createRPCallback;
    float exposure = 1.0f;

    std::vector<Components::MeshComponent*> _meshComponents;

    BufferHandle _uniformBuffer, _instanceBuffer;
    GLSLStruct* _uData, *_uInstance;
    std::unordered_map<std::string, GLSLStruct*> _uniformStructs;

    void CreateUniformBuffer();
    void CreateLineBuffer(uint32_t byteSize);
    void CreateRGBA16fFramebuffer();
    void Initialise();
    void InitialiseImGUI();
    void SetupDebugCallback();
    void UpdateUniformBuffers();
    void Render();
    void RenderSceneInspector();
    void RenderGUI();
    void RenderLine(LineSegment& line, uint32_t offset);
    void ResetFrameData();
    void CreateRenderpass();
    Renderer();

public:
    void InvalidateRenderpass();

    void AddShader(Shader* s);

    std::unordered_map<std::string, GLSLStruct*>& GetStdUniformStructs();

    void UpdateUniforms(Components::MeshComponent* comp);
    
    static Renderer* GetInstance();

    void SetScene(Engine::Core::Scene* scene);
    
    void RenderLoop();

    void SetMainCamera(Camera* camera);

    void SetRenderpass(Renderpass* rp);

    void SetRenderpassReconstructionCallback(std::function<Renderpass*()> func);

    PointLight* GetNewPointLight();

    void SetDirectionalLight(DirectionalLight* directionalLight);

    float GetAspectRatio();
    
    void GetGLErrors();

    void DrawLineSegment(LineSegment segment);

    void SetSkybox(Skybox* skybox);

    glm::vec2 GetWindowDimensions();

};

} // namespace Rendering
