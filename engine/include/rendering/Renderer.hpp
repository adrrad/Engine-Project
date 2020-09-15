#pragma once
#include "core/EngineSubsystem.hpp"

#include "utilities/Array.hpp"

#include "platform/WindowManager.hpp"
#include "rendering/GLSLStruct.hpp"
#include "rendering/Framebuffer.hpp"
#include "rendering/Renderpass.hpp"
#include "rendering/Shader.hpp"
#include "rendering/Mesh.hpp"
#include "core/Scene.hpp"
#include "rendering/RenderingTypedefs.hpp"
#include "rendering/RenderingStructs.hpp"
#include "rendering/GLSLStruct.hpp"

#include "components/MeshComponent.hpp"

#include <unordered_map>
#include <string>
#include <functional>

namespace Engine::Rendering
{

class Renderer : public Engine::Core::EngineSubsystem
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
    DirectionalLight *_directionalLight = nullptr;
    std::vector <Shader*> _shaders;


    Shader* _lineShader = nullptr;
    uint32_t _lineVAO = 0, _lineVBO = 0;
    uint32_t _currentLineVertexCount = 0;
    std::vector<LineInfo> _lineSegments;
    const uint32_t _maxLineVertexCount = 100000;
    Engine::Utilities::Array<glm::vec3>* _linedata;

    Renderpass* _rp = nullptr;
    std::function<Renderpass*()> _createRPCallback;
    std::vector<std::function<void()>> _guiDraws;

    std::vector<Components::MeshComponent*> _meshComponents;

    BufferHandle _uniformBuffer, _instanceBuffer;
    GLSLStruct* _uData;
    std::unordered_map<std::string, GLSLStruct*> _uniformStructs;

    void CreateUniformBuffer();
    void CreateLineBuffer(uint32_t byteSize);
    void CreateRGBA16fFramebuffer();
    void Initialise();
    void InitialiseImGUI();
    void SetupDebugCallback();
    void UpdateUniformBuffers();
    void Render();
    void RenderGUI();
    void ResetFrameData();
    void CreateRenderpass();
    Renderer();

public:
    void Update(float deltaTime) override {};

    void InvalidateRenderpass();

    void AddShader(Shader* s);

    std::unordered_map<std::string, GLSLStruct*>& GetStdUniformStructs();

    void UpdateUniforms(Components::MeshComponent* comp);
    
    static Renderer* GetInstance();

    void SetScene(Engine::Core::Scene* scene);
    
    void RenderLoop(std::function<void(float)> drawCall = nullptr);

    void SetMainCamera(Camera* camera);

    void SetRenderpass(Renderpass* rp);

    void SetRenderpassReconstructionCallback(std::function<Renderpass*()> func);

    PointLight* GetNewPointLight();

    void SetDirectionalLight(DirectionalLight* directionalLight);

    float GetAspectRatio();
    
    void GetGLErrors();

    void DrawLineSegment(LineSegment segment);

    void RegisterGUIDraw(std::function<void()> func);

    glm::vec2 GetWindowDimensions();

};

} // namespace Engine::Rendering
