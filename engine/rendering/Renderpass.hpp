#pragma once



#include "rendering/Renderqueue.hpp"
#include "rendering/RVM.hpp"
#include "rendering/Framebuffer.hpp"
#include "rendering/RenderingTypedefs.hpp"
#include "EngineComponent.hpp"

#include <glad/glad.h>

#include <vector>
#include <string>
#include <stdint.h>

template<class T>
class SubsystemComponent;

namespace Gameplay
{
    class MeshComponent;
    class LightComponent;
    class CameraComponent;
}

namespace Rendering
{
class Material;

enum class SubpassFlags
{
    DEFAULT = 0,
    DISABLE_DEPTHMASK = 1,
    ENABLE_BLENDING = 2,
};

inline SubpassFlags operator|(SubpassFlags a, SubpassFlags b)
{
    return static_cast<SubpassFlags>(static_cast<int>(a) | static_cast<int>(b));
}

inline SubpassFlags operator&(SubpassFlags a, SubpassFlags b)
{
    return static_cast<SubpassFlags>(static_cast<int>(a) & static_cast<int>(b));
}

class Renderpass : public SubsystemComponent<Renderer>
{
    friend class EngineComponent;
private:
    struct Subpass : public EngineComponent
    {
        std::string Name = "No Name";
        SubpassFlags Flags = SubpassFlags::DEFAULT;
        Renderqueue* Queue = nullptr;
        Subpass* Next = nullptr;
        Subpass(std::string name, SubpassFlags flags, Size renderQueueSize);
        ~Subpass();
        void StartSubpass();
        void EndSubpass();
    };

    Subpass* m_first = nullptr;
    u64 m_numSubpasses;
    RVM* m_machine = nullptr;

    Renderpass(Subpass* first, u64 numSubpasses);
public:
    ~Renderpass();

    void Execute();

    Renderqueue* GetRenderqueue();

    class RenderpassBuilder : public EngineComponent
    {
    friend class Renderpass;
    private:
        Subpass* m_first = nullptr;
        Subpass* m_currentSubpass = nullptr;
        u64 m_numSubpasses = 0;
        u64 m_totalInstructions = 0;
        u64 m_totalVariables = 0;
        ShaderID m_currentShader = 0;
        ActiveTextureID m_currentActiveTexture = GL_TEXTURE0;
        RenderpassBuilder();

    public:
        RenderpassBuilder& NewSubpass(std::string name, SubpassFlags flags = SubpassFlags::DEFAULT, Size renderQueueSize = 10000);
        RenderpassBuilder& UseFramebuffer(Framebuffer* fb);

        /**
         * @brief Binds textures associated with the given Framebuffer.
         * 
         * @param fb The Framebuffer object.
         * @param append Whether to append the textures after any previousy bound or to ignore them.
         * @return RenderpassBuilder& The builder object reference.
         */
        RenderpassBuilder& BindFramebufferTextures(Framebuffer* fb, bool append = false);
        RenderpassBuilder& ClearDepthBuffer();
        RenderpassBuilder& UseShader(ShaderID id);
        RenderpassBuilder& UseMaterial(Material* mat);
        RenderpassBuilder& UseCamera(Gameplay::CameraComponent* camera);
        RenderpassBuilder& BindBufferRange(u64 binding, BufferHandle buffer, VarOffset offset, u64 size);
        RenderpassBuilder& BindTexture(UniformID uid, ActiveTextureID aid, TextureID tid, TextureTarget tt);
        RenderpassBuilder& BindLight(Gameplay::LightComponent* light);
        RenderpassBuilder& BindMeshInstance(Gameplay::MeshComponent* comp);
        RenderpassBuilder& DrawMesh(uint32_t vao, uint32_t topology, uint32_t u64);
        RenderpassBuilder& DrawMesh(Gameplay::MeshComponent* comp, bool useMaterial = true);
        RenderpassBuilder& DrawMeshes(uint32_t count, uint32_t* vao, uint32_t* topology, uint32_t* u64);
        
        /**
         * @brief Perform a lighting pass. (deferred shading only)
         * 
         * @param light The Light Component to use.
         * @return RenderpassBuilder& The builder object reference.
         */
        RenderpassBuilder& RenderLightPass(Gameplay::LightComponent* light);

        Renderpass* Build(bool concatenateSubpasses = true);
    };

    static RenderpassBuilder Create(Memory::Allocator* allocator);

};

}