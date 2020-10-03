#pragma once
#include "core/EngineSubsystem.hpp"

#include "Array.hpp"

#include "EngineTypedefs.hpp"
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
#include "rendering/Framebuffer.hpp"

#include "components/MeshComponent.hpp"

#include <unordered_map>
#include <string>
#include <functional>

namespace Engine::Rendering
{

class Renderer : public Engine::Core::EngineSubsystem
{

private:
    uint32_t m_windowWidth = 1600;
    uint32_t m_windowHeight = 1024;
    static Renderer* _instance;
    Platform::WindowManager* _windowManager;
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
    Engine::Array<glm::vec3>* _linedata;

    Renderpass* _rp = nullptr;
    std::function<Renderpass*()> _createRPCallback;
    std::vector<std::function<void()>> _guiDraws;

    std::vector<Components::MeshComponent*> _meshComponents;

    GLSLStruct* _uData;
    std::unordered_map<std::string, GLSLStruct*> _uniformStructs;

    std::vector<Texture*> m_textures;
    std::vector<Mesh*> m_meshes;
    std::unordered_map<std::string, Texture*> m_textureMapping;
    std::unordered_map<std::string, Mesh*> m_meshMapping;

    Framebuffer* m_gBuffer = nullptr;
    Framebuffer* m_lightBuffer = nullptr;
    Mesh* m_targetQuad = nullptr;
    Mesh* m_skybox = nullptr;
    Shader* m_lightShader = nullptr;
    Shader* m_skyboxShader = nullptr;
    Material* m_lightMaterial = nullptr;
    Material* m_skyboxMaterial = nullptr;
    Components::MeshComponent m_lightMC;
    Components::MeshComponent m_skyboxMC;
    Renderpass* m_renderpass = nullptr;

    void CreateUniformBuffer();
    void CreateLineBuffer(uint32_t byteSize);
    void Initialise();
    void InitialiseImGUI();
    void SetupDebugCallback();
    void InitialiseDeferredShading();
    void CreateFramebuffers();
    void RecreateFramebuffers();
    void UpdateUniformBuffers();
    void Render();
    void RenderGUI();
    void ResetFrameData();
    Renderer();

public:

    static Renderer* GetInstance();

    void Update(float deltaTime) override {};

    void InvalidateRenderpass();

    void AddShader(Shader* s);

    Mesh* GetMesh(AssetID meshAssetID);

    Texture* GetTexture(AssetID imageAssetID);

    std::unordered_map<std::string, GLSLStruct*>& GetStdUniformStructs();
    
    void UpdateUniforms(Components::MeshComponent* comp);

    void RecordScene(Core::Scene* scene);
    
    void RenderFrame();

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
