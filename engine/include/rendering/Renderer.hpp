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
#include "rendering/Cubemap.hpp"
#include "rendering/Light.hpp"

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
    static Renderer* m_instance;
    Platform::WindowManager* m_windowManager;
    uint32_t m_activeWindow;
    float m_totalTime = 0;
    Camera *m_mainCamera = nullptr;
    std::vector<PointLight*> m_pointLights;
    std::vector<DirectionalLight*> m_dirLights;
    std::vector<Camera*> m_cameras;
    DirectionalLight *m_directionalLight = nullptr;
    std::vector <Shader*> m_shaders;


    Shader* m_lineShader = nullptr;
    uint32_t m_lineVAO = 0, m_lineVBO = 0;
    uint32_t m_currentLineVertexCount = 0;
    std::vector<LineInfo> m_lineSegments;
    const uint32_t m_maxLineVertexCount = 100000;
    Engine::Array<glm::vec3>* m_linedata;

    Renderpass* m_rp = nullptr;
    std::function<Renderpass*()> m_createRPCallback;
    std::vector<std::function<void()>> m_guiDraws;

    std::vector<Components::MeshComponent*> m_meshComponents;

    GLSLStruct* m_uData;
    GLSLStruct* m_uLights;
    GLSLStruct* m_udirLights;
    GLSLStruct* m_cameraBuffer;
    std::unordered_map<std::string, GLSLStruct*> m_uniformStructs;

    std::vector<Texture*> m_textures;
    std::vector<Cubemap*> m_cubemaps;
    std::vector<Mesh*> m_meshes;
    std::unordered_map<std::string, Texture*> m_textureMapping;
    std::unordered_map<std::string, Cubemap*> m_cubemapMapping;
    std::unordered_map<std::string, Mesh*> m_meshMapping;

    Framebuffer* m_gBuffer = nullptr;
    Framebuffer* m_lightBuffer = nullptr;
    Mesh* m_targetQuad = nullptr;
    Shader* m_lightShader = nullptr;
    Material* m_lightMaterial = nullptr;
    Components::MeshComponent m_lightMC;
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
    Texture* CreateTexture(AssetID imageAssetID);
    Mesh* CreateMesh(AssetID meshAssetID);
    Renderer();

public:

    static Renderer* GetInstance();

    void Update(float deltaTime) override {};

    void InvalidateRenderpass();

    void AddShader(Shader* s);

    Shader* GetShader(std::string name);

    Mesh* GetMesh(AssetID meshAssetID);

    Texture* GetTexture(AssetID imageAssetID);

    Cubemap* GetCubemap(AssetID cubemapAssetID);

    std::unordered_map<std::string, GLSLStruct*>& GetStdUniformStructs();
    
    void UpdateUniforms(Components::MeshComponent* comp);

    void RecordScene(Core::Scene* scene);
    
    void RenderFrame();

    void SetMainCamera(Camera* camera);

    void SetRenderpass(Renderpass* rp);

    void SetRenderpassReconstructionCallback(std::function<Renderpass*()> func);

    Camera* GetNewCamera(Buffer* buffer);

    PointLight* GetNewPointLight(Buffer* buffer);

    DirectionalLight* GetNewDirectionalLight(Buffer* buffer);

    // void SetDirectionalLight(DirectionalLight* directionalLight);

    float GetAspectRatio();
    
    void GetGLErrors();

    void DrawLineSegment(LineSegment segment);

    void RegisterGUIDraw(std::function<void()> func);

    glm::vec2 GetWindowDimensions();

};

} // namespace Engine::Rendering
