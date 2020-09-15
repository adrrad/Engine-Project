#include "rendering/Renderpass.hpp"

#include "rendering/Shader.hpp"

#include "rendering/Debugging.hpp"
#include "components/MeshComponent.hpp"

#include <iostream>
#include <cstring>

void print(const char* text)
{
    std::cout << text << std::endl;
}

namespace Rendering
{
typedef Renderpass::RenderpassBuilder RenderpassBuilder;

Renderpass::Subpass::Subpass(std::string name, SubpassFlags flags, Size renderQueueSize)
{
    Name = name;
    Flags = flags;
    if(renderQueueSize > 0) 
    {
        Queue = new Renderqueue(renderQueueSize);
        StartSubpass();
    }
}

Renderpass::Subpass::~Subpass()
{
    //Recursively delete subpasses, if any
    delete Queue;
    if(Next) delete Next;
}

void Renderpass::Subpass::StartSubpass()
{
    if(int(Flags & SubpassFlags::DISABLE_DEPTHMASK)) Queue->PushInstruction(MachineCode::DISABLE_DEPTHMASK);
    if(int(Flags & SubpassFlags::ENABLE_BLENDING)) Queue->PushInstruction(MachineCode::ENABLE_BLENDING);
}

void Renderpass::Subpass::EndSubpass()
{
    if(int(Flags & SubpassFlags::DISABLE_DEPTHMASK)) Queue->PushInstruction(MachineCode::ENABLE_DEPTHMASK);
    if(int(Flags & SubpassFlags::ENABLE_BLENDING)) Queue->PushInstruction(MachineCode::DISABLE_BLENDING);
}

Renderpass::Renderpass(Subpass* first, ElementCount numSubpasses)
{
    _machine = new Machine();
    _first = first;
}

Renderpass::~Renderpass()
{
    delete _first;
    delete _machine;
}

RenderpassBuilder::RenderpassBuilder()
{

}

RenderpassBuilder& RenderpassBuilder::NewSubpass(std::string name, SubpassFlags flags, Size renderQueueSize)
{
    if(_numSubpasses > 0){
        _currentSubpass->EndSubpass();
        _totalInstructions += _currentSubpass->Queue->GetInstructionsCount();
        _totalVariables += _currentSubpass->Queue->GetVariablesCount();
    } 
    if(_first == nullptr)
    {
        _first = _currentSubpass = new Subpass(name, flags, renderQueueSize);
    } 
    else
    {
        _currentSubpass->Next = new Subpass(name, flags, renderQueueSize);
        _currentSubpass = _currentSubpass->Next;
    }
    _numSubpasses++;
    return *this;
}

RenderpassBuilder& RenderpassBuilder::UseFramebuffer(Framebuffer* fb)
{
    _currentSubpass->Queue->UseFramebuffer(fb->GetFBO());
    return *this;
}

RenderpassBuilder& RenderpassBuilder::ClearDepthBuffer()
{
    _currentSubpass->Queue->PushInstruction(MachineCode::CLEAR_DEPTH_BUFFER);
    return *this;
}

RenderpassBuilder& RenderpassBuilder::UseShader(ShaderID id)
{
    _currentSubpass->Queue->PushInstruction(MachineCode::USE_SHADER);
    _currentSubpass->Queue->PushVariable(id);
    return *this;
}

RenderpassBuilder& RenderpassBuilder::BindBufferRange(Index binding, BufferHandle buffer, VarOffset offset, SizeBytes size)
{
    _currentSubpass->Queue->PushInstruction(MachineCode::BIND_UNIFORM_BUFFER_RANGE);
    _currentSubpass->Queue->PushVariable(binding);
    _currentSubpass->Queue->PushVariable(buffer);
    _currentSubpass->Queue->PushVariable(offset);
    _currentSubpass->Queue->PushVariable(size);
    return *this;
}

RenderpassBuilder& RenderpassBuilder::BindTexture(UniformID uid, ActiveTextureID aid, TextureID tid, TextureTarget tt)
{
    _currentSubpass->Queue->PushInstruction(MachineCode::BIND_TEXTURE);
    _currentSubpass->Queue->PushVariable(uid);
    _currentSubpass->Queue->PushVariable(aid);
    _currentSubpass->Queue->PushVariable(tid);
    _currentSubpass->Queue->PushVariable(tt);
    return *this;
}


RenderpassBuilder& RenderpassBuilder::DrawMesh(uint32_t vao, uint32_t topology, uint32_t elementCount)
{
    _currentSubpass->Queue->Push(vao, topology, elementCount);
    return *this;
}

RenderpassBuilder& RenderpassBuilder::DrawMeshes(uint32_t count, uint32_t* vao, uint32_t* topology, uint32_t* elementCount)
{
    for(uint32_t meshIndex = 0; meshIndex < count; meshIndex++)
    {
        _currentSubpass->Queue->Push(vao[meshIndex], topology[meshIndex], elementCount[meshIndex]);
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
    BufferHandle vao = mat->GetVAO();
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

Renderpass* RenderpassBuilder::Build(bool concatenateSubpasses)
{
    if(_numSubpasses < 0) throw std::exception("Cannot create a render pass without any subpasses!");//TODO: exception if no subpasses
    _currentSubpass->EndSubpass();

    if(concatenateSubpasses)
    {
        MachineCode* instructions = new MachineCode[_totalInstructions];
        Variable* variables = new Variable[_totalVariables];
        ElementCount numInstructions = 0, numVariables = 0;
        Subpass* current = _first;
        size_t iSize = sizeof(MachineCode);
        size_t vSize = sizeof(Variable);
        while(current != nullptr)
        {
            ElementCount numI = current->Queue->GetInstructionsCount();
            ElementCount numV = current->Queue->GetVariablesCount();

            memcpy(instructions+numInstructions, current->Queue->GetInstructions(), iSize*numI);
            memcpy(variables+numVariables, current->Queue->GetVariables(), vSize*numV);

            numInstructions += numI;
            numVariables += numV;
            current = current->Next;
        }
        delete _first;
        current = new Subpass("Concatanated", SubpassFlags::DEFAULT, 0);
        current->Queue = new Renderqueue(instructions, variables, numInstructions, numVariables);
        return new Renderpass(current, 1);
    }
    return new Renderpass(_first, _numSubpasses);
}

RenderpassBuilder Renderpass::Create()
{
    return RenderpassBuilder();
}


void Renderpass::Execute()
{
    Subpass* current = _first;
    while(current != nullptr)
    {
        _machine->Run(current->Queue);
        current = current->Next;
    } 
}

}