#pragma once
#include "core/EngineSubsystem.hpp"

#include "Array.hpp"

#include "EngineTypedefs.hpp"
#include "EngineComponent.hpp"
#include "platform/WindowManager.hpp"
#include "rendering/GLSLStruct.hpp"
#include "rendering/Framebuffer.hpp"
#include "rendering/Renderpass.hpp"
#include "rendering/Shader.hpp"
#include "rendering/Mesh.hpp"
#include "gameplay/Scene.hpp"
#include "rendering/RenderingTypedefs.hpp"
#include "rendering/RenderingStructs.hpp"
#include "rendering/GLSLStruct.hpp"
#include "rendering/Framebuffer.hpp"
#include "rendering/Cubemap.hpp"

#include "components/MeshComponent.hpp"

#include <unordered_map>
#include <string>
#include <functional>

namespace Memory 
{
    template<class T>
    class PoolAllocator;
}

namespace Rendering
{

struct RendererSettings
{
    u32 WindowWidth = 0;
    u32 WindowHeight = 0;
    u32 MaxCameraCount = 0;
    u32 MaxPointLightCount = 0;
    u32 MaxDirectionalLightCount = 0;
    u32 MaxLineVertexCount = 0;
};



class Renderer : public EngineSubsystem<Renderer>
{
friend class EngineSubsystem<Renderer>;
private:
    uint32_t m_windowWidth = 1600;
    uint32_t m_windowHeight = 1024;
    Platform::WindowManager* m_windowManager;
    uint32_t m_activeWindow;
    float m_totalTime = 0;
    Camera *m_mainCamera = nullptr;
    std::vector<PointLight*> m_pointLights;
    std::vector<DirectionalLight*> m_dirLights;
    std::vector<Camera*> m_cameras;
    std::vector <Shader*> m_shaders;


    Shader* m_lineShader = nullptr;
    uint32_t m_lineVAO = 0, m_lineVBO = 0;
    uint32_t m_currentLineVertexCount = 0;
    std::vector<LineInfo> m_lineSegments;
    const uint32_t m_maxLineVertexCount = 100000;
    Array<glm::vec3>* m_linedata;

    std::vector<std::function<void()>> m_guiDraws;

    std::vector<Gameplay::MeshComponent*> m_meshComponents;

    std::vector<Texture*> m_textures;
    std::vector<Cubemap*> m_cubemaps;
    std::vector<Mesh*> m_meshes;
    std::unordered_map<std::string, Texture*> m_textureMapping;
    std::unordered_map<std::string, Cubemap*> m_cubemapMapping;
    std::unordered_map<std::string, Mesh*> m_meshMapping;

    void CreateLineBuffer(uint32_t byteSize);

    void InitialiseImGUI();

    void SetupDebugCallback();

    Texture* CreateTexture(AssetID imageAssetID);

    Mesh* CreateMesh(AssetID meshAssetID);

    Renderer();

public:


    void UpdateUniformBuffers();

    void Initialise() override;

    void AddShader(Shader* s);

    Shader* GetShader(std::string name);

    Mesh* GetMesh(AssetID meshAssetID);

    Texture* GetTexture(AssetID imageAssetID);

    Cubemap* GetCubemap(AssetID cubemapAssetID);
    
    void RenderLines();

    void RenderGUI();

    /**
     * @brief Creates a Camera object and stores the related buffer information.
     * 
     * @param buffer An emtpy (invalid) buffer object.
     * @return Camera* Pointer to the camera object
     */
    Camera* GetNewCamera(BufferRange* buffer);

    /**
     * @brief Creates a new Point Light object and stores the related buffer information.
     * 
     * @param buffer An emtpy (invalid) buffer object.
     * @return PointLight* Pointer to the Point Light object.
     */
    PointLight* GetNewPointLight(BufferRange* buffer);

    /**
     * @brief Creates a new Directional Light object and stores the related buffer information.
     * 
     * @param buffer An emtpy (invalid) buffer object.
     * @return DirectionalLight* A pointer to the Directional Light Object.
     */
    DirectionalLight* GetNewDirectionalLight(BufferRange* buffer);

    /**
     * @brief Creates a buffer with the specified size and data.
     * 
     * @param buffer An empty buffer object.
     * @param size The size of the buffer in bytes.
     * @param data A pointer to the data to be uploaded to the buffer.
     * @param usage GLEnum specifying the buffer usage (default: GL_DYNAMIC_DRAW).
     * @return Buffer A Buffer object with uniform buffer information.
     */
    void CreateUniformBuffer(Buffer &buffer, Size size, void* data, int usage = GL_DYNAMIC_DRAW);

    /**
     * @brief Frees the allocated uniform buffer resource and invalidates the buffer object.
     * 
     * @param buffer Reference to the Buffer object containing the uniform buffer information.
     */
    void FreeUniformBuffer(Buffer& buffer);

    float GetAspectRatio();
    
    void GetGLErrors();

    void DrawLineSegment(LineSegment segment);

    void RegisterGUIDraw(std::function<void()> func);

    glm::vec2 GetWindowDimensions();

};

} // namespace Rendering
