#pragma once


#include "renderer/Renderqueue.hpp"
#include "renderer/Machine.hpp"
#include "renderer/Framebuffer.hpp"


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

    class RenderpassBuilder
    {
    friend class Renderpass;
    private:
        std::vector<Subpass> _subpasses;

        RenderpassBuilder();

    public:
        RenderpassBuilder& NewSubpass(std::string name, SubpassFlags flags = SubpassFlags::DEFAULT);
        RenderpassBuilder& UseFramebuffer(Framebuffer* fb);
        RenderpassBuilder& DrawMesh(uint32_t vao, uint32_t topology, uint32_t elementCount, Components::MeshComponent* mat);
        RenderpassBuilder& DrawMeshes(uint32_t count, uint32_t* vao, uint32_t* topology, uint32_t* elementCount, Components::MeshComponent** mat);
        Renderpass* Build();
    };

    static RenderpassBuilder Create();

    void Execute();

};

}