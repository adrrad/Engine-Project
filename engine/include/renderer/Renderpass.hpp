#pragma once


#include "renderer/Renderqueue.hpp"
#include "renderer/Machine.hpp"
#include "renderer/Framebuffer.hpp"
#include "renderer/RenderingTypedefs.hpp"

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
        std::string Name;
        SubpassFlags Flags;
        Renderqueue* Queue;
        Subpass(std::string name, SubpassFlags flags);
        void StartSubpass();
        void EndSubpass();
        
    };

    std::vector<Subpass> _subpasses;
    Machine* _machine;

    Renderpass(std::vector<Subpass> subpasses);
public:
    ~Renderpass();

    void Execute();

    class RenderpassBuilder
    {
    friend class Renderpass;
    private:
        std::vector<Subpass> _subpasses;
        Subpass* _currentSubpass;
        RenderpassBuilder();

    public:
        RenderpassBuilder& NewSubpass(std::string name, SubpassFlags flags = SubpassFlags::DEFAULT);
        RenderpassBuilder& UseFramebuffer(Framebuffer* fb);
        RenderpassBuilder& ClearDepthBuffer();
        RenderpassBuilder& UseShader(ShaderID id);
        RenderpassBuilder& BindBufferRange(Index binding, BufferHandle buffer, VarOffset offset, SizeBytes size);
        RenderpassBuilder& BindTexture(UniformID uid, ActiveTextureID aid, TextureID tid, TextureTarget tt);
        RenderpassBuilder& DrawMesh(uint32_t vao, uint32_t topology, uint32_t elementCount);
        RenderpassBuilder& DrawMesh(Components::MeshComponent* comp);
        RenderpassBuilder& DrawMeshes(uint32_t count, uint32_t* vao, uint32_t* topology, uint32_t* elementCount);

        Renderpass* Build();
    };

    static RenderpassBuilder Create();

};

}