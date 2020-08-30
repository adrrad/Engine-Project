#include "renderer/Renderpass.hpp"

#include "components/MeshComponent.hpp"

#include <iostream>

namespace Rendering
{
typedef Renderpass::RenderpassBuilder RenderpassBuilder;

Renderpass::Subpass::Subpass(std::string name, SubpassFlags flags)
{
    Name = name;
    Flags = flags;
    Queue = new Renderqueue(1000);
    StartSubpass();
}

void Renderpass::Subpass::StartSubpass()
{
    if(int(Flags & SubpassFlags::DISABLE_DEPTHMASK)) Queue->UseDepthMask(false);
}

void Renderpass::Subpass::EndSubpass()
{
    if(int(Flags & SubpassFlags::DISABLE_DEPTHMASK)) Queue->UseDepthMask(true);
}

Renderpass::Renderpass(std::vector<Subpass> subpasses)
{
    _machine = new Machine();
    _subpasses = subpasses;
}

RenderpassBuilder::RenderpassBuilder()
{

}

RenderpassBuilder& RenderpassBuilder::NewSubpass(std::string name, SubpassFlags flags)
{
    if(_subpasses.size() > 0) _subpasses.back().EndSubpass();
    _subpasses.push_back(Subpass(name, flags));
    return *this;
}

RenderpassBuilder& RenderpassBuilder::UseFramebuffer(Framebuffer* fb)
{
    _subpasses.back().Queue->UseFramebuffer(fb->GetFBO());
    return *this;
}

RenderpassBuilder& RenderpassBuilder::UseShader(ShaderID id)
{
    _subpasses.back().Queue->PushInstruction(MachineCode::USE_SHADER);
    _subpasses.back().Queue->PushVariable(id);
    return *this;
}

RenderpassBuilder& RenderpassBuilder::BindBufferRange(Index binding, BufferHandle buffer, VarOffset offset, SizeBytes size)
{
    _subpasses.back().Queue->PushInstruction(MachineCode::BIND_UNIFORM_BUFFER_RANGE);
    _subpasses.back().Queue->PushVariable(binding);
    _subpasses.back().Queue->PushVariable(buffer);
    _subpasses.back().Queue->PushVariable(offset);
    _subpasses.back().Queue->PushVariable(size);
    return *this;
}

RenderpassBuilder& RenderpassBuilder::BindTexture(UniformID uid, ActiveTextureID aid, TextureID tid, TextureTarget tt)
{
    _subpasses.back().Queue->PushInstruction(MachineCode::BIND_TEXTURE);
    _subpasses.back().Queue->PushVariable(uid);
    _subpasses.back().Queue->PushVariable(aid);
    _subpasses.back().Queue->PushVariable(tid);
    _subpasses.back().Queue->PushVariable(tt);
    return *this;
}


RenderpassBuilder& RenderpassBuilder::DrawMesh(uint32_t vao, uint32_t topology, uint32_t elementCount, Components::MeshComponent* mat)
{
    _subpasses.back().Queue->Push(vao, topology, elementCount, mat);
    return *this;
}

RenderpassBuilder& RenderpassBuilder::DrawMeshes(uint32_t count, uint32_t* vao, uint32_t* topology, uint32_t* elementCount, Components::MeshComponent** mat)
{
    for(uint32_t meshIndex = 0; meshIndex < count; meshIndex++)
    {
        _subpasses.back().Queue->Push(vao[meshIndex], topology[meshIndex], elementCount[meshIndex], mat[meshIndex]);
    }
    return *this;
}

Renderpass* RenderpassBuilder::Build()
{
    return new Renderpass(_subpasses);
}

RenderpassBuilder Renderpass::Create()
{
    return RenderpassBuilder();
}


void Renderpass::Execute()
{
    uint32_t size = _subpasses.size();
    for(uint32_t spIndex = 0; spIndex < size; spIndex++)
    {
        _machine->Run(_subpasses[spIndex].Queue);
    }
}


}