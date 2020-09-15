#pragma once


#include "rendering/Renderqueue.hpp"
#include "rendering/Machine.hpp"
#include "rendering/Framebuffer.hpp"
#include "rendering/RenderingTypedefs.hpp"

#include <vector>
#include <string>
#include <stdint.h>

namespace Components
{
    class MeshComponent;
}

namespace Rendering
{

class Foo {
public:
  enum {BAR, BAZ};
};

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

class Renderpass
{
private:
    struct Subpass
    {
        std::string Name = "No Name";
        SubpassFlags Flags = SubpassFlags::DEFAULT;
        Renderqueue* Queue = nullptr;
        Subpass(std::string name, SubpassFlags flags, Size renderQueueSize);
        ~Subpass();
        void StartSubpass();
        void EndSubpass();
        Subpass* Next = nullptr;
    };

    Subpass* _first;
    ElementCount _numSubpasses;
    Machine* _machine;

    Renderpass(Subpass* first, ElementCount numSubpasses);
public:
    ~Renderpass();

    void Execute();

    class RenderpassBuilder
    {
    friend class Renderpass;
    private:
        Subpass* _first = nullptr;
        Subpass* _currentSubpass = nullptr;
        ElementCount _numSubpasses = 0;
        ElementCount _totalInstructions = 0;
        ElementCount _totalVariables = 0;
        RenderpassBuilder();

    public:
        RenderpassBuilder& NewSubpass(std::string name, SubpassFlags flags = SubpassFlags::DEFAULT, Size renderQueueSize = 10000);
        RenderpassBuilder& UseFramebuffer(Framebuffer* fb);
        RenderpassBuilder& ClearDepthBuffer();
        RenderpassBuilder& UseShader(ShaderID id);
        RenderpassBuilder& BindBufferRange(Index binding, BufferHandle buffer, VarOffset offset, SizeBytes size);
        RenderpassBuilder& BindTexture(UniformID uid, ActiveTextureID aid, TextureID tid, TextureTarget tt);
        RenderpassBuilder& DrawMesh(uint32_t vao, uint32_t topology, uint32_t elementCount);
        RenderpassBuilder& DrawMesh(Components::MeshComponent* comp);
        RenderpassBuilder& DrawMeshes(uint32_t count, uint32_t* vao, uint32_t* topology, uint32_t* elementCount);

        Renderpass* Build(bool concatenateSubpasses = false);
    };

    static RenderpassBuilder Create();

};

}