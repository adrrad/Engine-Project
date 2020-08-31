#include "renderer/Renderpass.hpp"

#include "renderer/Debugging.hpp"
#include "components/MeshComponent.hpp"


#include <iostream>

void print(const char* text)
{
    std::cout << text << std::endl;
}

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
    if(int(Flags & SubpassFlags::DISABLE_DEPTHMASK)) Queue->PushInstruction(MachineCode::DISABLE_DEPTHMASK);
}

void Renderpass::Subpass::EndSubpass()
{
    if(int(Flags & SubpassFlags::DISABLE_DEPTHMASK)) Queue->PushInstruction(MachineCode::ENABLE_DEPTHMASK);
}

Renderpass::Renderpass(std::vector<Subpass> subpasses)
{
    _machine = new Machine();
    _subpasses = subpasses;
}

Renderpass::~Renderpass()
{
    for(auto& subpass : _subpasses) delete subpass.Queue;
    delete _machine;
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

RenderpassBuilder& RenderpassBuilder::ClearDepthBuffer()
{
    _subpasses.back().Queue->PushInstruction(MachineCode::CLEAR_DEPTH_BUFFER);
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


RenderpassBuilder& RenderpassBuilder::DrawMesh(uint32_t vao, uint32_t topology, uint32_t elementCount)
{
    _subpasses.back().Queue->Push(vao, topology, elementCount);
    return *this;
}

RenderpassBuilder& RenderpassBuilder::DrawMeshes(uint32_t count, uint32_t* vao, uint32_t* topology, uint32_t* elementCount)
{
    for(uint32_t meshIndex = 0; meshIndex < count; meshIndex++)
    {
        _subpasses.back().Queue->Push(vao[meshIndex], topology[meshIndex], elementCount[meshIndex]);
    }
    return *this;
}

RenderpassBuilder& RenderpassBuilder::DrawMesh(Components::MeshComponent* comp)
{
    if(comp->_mesh == nullptr) throw std::exception("A mesh component must have a mesh attached before rendering!");
    //Retrieve necessary data
    Mesh* mesh =  comp->_mesh;
    Material* mat = comp->_material;
    Shader* shader = mat->_shader;
    ShaderID program = shader->GetID();
    Index instanceIndex = mat->_instanceIndex;
    BufferHandle vao = mesh->GetVAO();
    ElementCount indexCount = mesh->GetIndexCount();
    UseShader(program);
    
    //Bind uniform buffers
    for(auto p : shader->_uniformBlocks) 
    {
        GLSLStruct* str = p.second;
        Index bindingIndex = str->BindingIndex;
        BufferHandle uniformBuffer = str->GetUniformBuffer();
        VarOffset offset = str->GetInstanceOffset(instanceIndex);
        StructSize size = str->Size;
        BindBufferRange(bindingIndex, uniformBuffer, offset, size);
    }

    //Bind textures
    ActiveTextureID activeTexture = GL_TEXTURE0;
    for(auto pair : mat->_textures)
    {
        
        std::string name = pair.first;
        Texture* texture = pair.second;
        int uniformLocation = shader->ULoc(name);
        UPDATE_CALLINFO2("Uniform name: " + name);
        BindTexture(uniformLocation, activeTexture, texture->GetID(), texture->GetType());
        activeTexture+=1;
    }
    //Draw mesh
    DrawMesh(vao, GL_TRIANGLES, indexCount);
    return *this;
}

Renderpass* RenderpassBuilder::Build()
{
    if(_subpasses.size() > 0) _subpasses.back().EndSubpass();
    else throw std::exception("Cannot create a render pass without any subpasses!");//TODO: exception if no subpasses
    return new Renderpass(_subpasses);
}

RenderpassBuilder Renderpass::Create()
{
    return RenderpassBuilder();
}


void Renderpass::Execute()
{
    for(auto& sb : _subpasses)
    {
        auto name = sb.Name;
        _machine->Run(sb.Queue);
    } 
}

}