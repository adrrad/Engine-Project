#include "rendering/Renderpass.hpp"

#include "rendering/Shader.hpp"

#include "rendering/Debugging.hpp"
#include "components/MeshComponent.hpp"
#include "components/LightComponent.hpp"
#include "components/CameraComponent.hpp"

#include <iostream>
#include <cstring>


namespace Engine::Rendering
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
    m_machine = new Machine();
    m_first = first;
}

Renderpass::~Renderpass()
{
    delete m_first;
    delete m_machine;
}

RenderpassBuilder::RenderpassBuilder()
{

}

RenderpassBuilder& RenderpassBuilder::NewSubpass(std::string name, SubpassFlags flags, Size renderQueueSize)
{
    if(m_numSubpasses > 0){
        m_currentSubpass->EndSubpass();
        m_totalInstructions += m_currentSubpass->Queue->GetInstructionsCount();
        m_totalVariables += m_currentSubpass->Queue->GetVariablesCount();
    } 
    if(m_first == nullptr)
    {
        m_first = m_currentSubpass = new Subpass(name, flags, renderQueueSize);
    } 
    else
    {
        m_currentSubpass->Next = new Subpass(name, flags, renderQueueSize);
        m_currentSubpass = m_currentSubpass->Next;
    }
    m_numSubpasses++;
    m_currentShader = 0;
    return *this;
}

RenderpassBuilder& RenderpassBuilder::UseFramebuffer(Framebuffer* fb)
{
    glm::vec4 dims = fb->GetViewportDimensions();
    m_currentSubpass->Queue->UseFramebuffer(fb->GetFBO());
    m_currentSubpass->Queue->PushInstruction(MachineCode::SET_VIEWPORT);
    m_currentSubpass->Queue->PushVariable(dims.x);
    m_currentSubpass->Queue->PushVariable(dims.y);
    m_currentSubpass->Queue->PushVariable(dims.z);
    m_currentSubpass->Queue->PushVariable(dims.w);
    return *this;
}

RenderpassBuilder& RenderpassBuilder::BindFramebufferTextures(Framebuffer* fb, bool append)
{
    //TODO: Figure out how to handle active texture IDs for textures shared between shaders
    if(!append) m_currentActiveTexture = GL_TEXTURE0;
    // ActiveTextureID activeID = GL_TEXTURE0;
    for(auto& name : fb->GetColorBufferNames())
    {
        Texture* t = fb->GetColorbuffer(name);
        std::string fullname = fb->GetName() + "." + name;
        int location = glGetUniformLocation(m_currentShader, fullname.c_str());
        BindTexture(location, m_currentActiveTexture, t->GetID(), t->GetType());
        m_currentActiveTexture++;
    }
    for(auto& name : fb->GetDepthBufferNames())
    {
        Texture* t = fb->GetDepthBuffer(name);
        std::string fullname = fb->GetName() + "." + name;
        int location = glGetUniformLocation(m_currentShader, fullname.c_str());
        BindTexture(location, m_currentActiveTexture, t->GetID(), t->GetType());
        m_currentActiveTexture++;
    }
    return *this;
}

RenderpassBuilder& RenderpassBuilder::ClearDepthBuffer()
{
    m_currentSubpass->Queue->PushInstruction(MachineCode::CLEAR_DEPTH_BUFFER);
    return *this;
}

RenderpassBuilder& RenderpassBuilder::UseShader(ShaderID id)
{
    m_currentSubpass->Queue->PushInstruction(MachineCode::USE_SHADER);
    m_currentSubpass->Queue->PushVariable(id);
    m_currentShader = id;
    return *this;
}

RenderpassBuilder& RenderpassBuilder::UseMaterial(Material* mat)
{
    UseShader(mat->m_shader->ID);
    //Bind uniform buffers
    for(auto p : mat->m_shader->m_uniformBlocks) 
    {
        GLSLStruct* str = p.second;
        Index bindingIndex = str->BindingIndex;
        BufferHandle uniformBuffer = str->GetUniformBuffer();
        VarOffset offset = str->GetInstanceOffset(mat->m_instanceIndex);
        StructSize size = str->Size;
        BindBufferRange(bindingIndex, uniformBuffer, offset, size);
    }

    //Bind textures
    ActiveTextureID activeTexture = GL_TEXTURE0;
    for(auto pair : mat->m_textures)
    {
        std::string name = pair.first;
        Texture* texture = pair.second;
        int uniformLocation = mat->m_shader->ULoc(name);
        UPDATE_CALLINFO2("Uniform name: " + name);
        BindTexture(uniformLocation, activeTexture, texture->GetID(), texture->GetType());
        activeTexture+=1;
    }
    return *this;
}

RenderpassBuilder& RenderpassBuilder::UseCamera(Components::CameraComponent* camera)
{
    BufferRange& buffer = camera->m_cameraBuffer;
    BindBufferRange(buffer.BindingIndex, buffer.Handle, buffer.Offset, buffer.Size);
    return *this;
}

RenderpassBuilder& RenderpassBuilder::BindBufferRange(Index binding, BufferHandle buffer, VarOffset offset, SizeBytes size)
{
    m_currentSubpass->Queue->PushInstruction(MachineCode::BIND_UNIFORM_BUFFER_RANGE);
    m_currentSubpass->Queue->PushVariable(binding);
    m_currentSubpass->Queue->PushVariable(buffer);
    m_currentSubpass->Queue->PushVariable(offset);
    m_currentSubpass->Queue->PushVariable(size);
    return *this;
}

RenderpassBuilder& RenderpassBuilder::BindTexture(UniformID uid, ActiveTextureID aid, TextureID tid, TextureTarget tt)
{
    m_currentSubpass->Queue->PushInstruction(MachineCode::BIND_TEXTURE);
    m_currentSubpass->Queue->PushVariable(uid);
    m_currentSubpass->Queue->PushVariable(aid);
    m_currentSubpass->Queue->PushVariable(tid);
    m_currentSubpass->Queue->PushVariable(tt);
    return *this;
}

RenderpassBuilder& RenderpassBuilder::BindLight(Components::LightComponent* light)
{
    auto& lightBuffer = light->m_lightBuffer;
    int location = glGetUniformLocation(m_currentShader, "type");
    m_currentSubpass->Queue->PushInstruction(MachineCode::SET_UNIFORM_INT);
    m_currentSubpass->Queue->PushVariable(location);
    m_currentSubpass->Queue->PushVariable(Variable(light->GetType()));
    BindBufferRange(lightBuffer.BindingIndex, lightBuffer.Handle, lightBuffer.Offset, lightBuffer.Size);
    return *this;
}

RenderpassBuilder& RenderpassBuilder::BindMeshInstance(Components::MeshComponent* comp)
{
    auto material = comp->m_material;
    auto str = GLSLStruct::Get("InstanceUniforms");
    Index bindingIndex = str->BindingIndex;
    BufferHandle uniformBuffer = str->GetUniformBuffer();
    VarOffset offset = str->GetInstanceOffset(material->m_instanceIndex);
    StructSize size = str->Size;
    BindBufferRange(bindingIndex, uniformBuffer, offset, size);
    return *this;
}

RenderpassBuilder& RenderpassBuilder::DrawMesh(uint32_t vao, uint32_t topology, uint32_t elementCount)
{
    m_currentSubpass->Queue->Push(vao, topology, elementCount);
    return *this;
}

RenderpassBuilder& RenderpassBuilder::DrawMeshes(uint32_t count, uint32_t* vao, uint32_t* topology, uint32_t* elementCount)
{
    for(uint32_t meshIndex = 0; meshIndex < count; meshIndex++)
    {
        m_currentSubpass->Queue->Push(vao[meshIndex], topology[meshIndex], elementCount[meshIndex]);
    }
    return *this;
}

RenderpassBuilder& RenderpassBuilder::RenderLightPass(Components::LightComponent* light)
{
    static Mesh* quad = Mesh::GetQuad();
    BindLight(light);
    DrawMesh(quad->GetVAO(), GL_TRIANGLES, quad->GetIndexCount());
    return *this;
}

RenderpassBuilder& RenderpassBuilder::DrawMesh(Components::MeshComponent* comp, bool useMaterial)
{
    if(comp->m_mesh == nullptr) throw std::exception("A mesh component must have a mesh attached before rendering!");
    //Retrieve necessary data
    Mesh* mesh =  comp->m_mesh;
    Material* mat = comp->m_material;
    BufferHandle vao = mat->GetVAO();
    uint32_t indexCount = mesh->GetIndexCount();
    if(useMaterial) UseMaterial(mat);
    DrawMesh(vao, GL_TRIANGLES, indexCount);
    return *this;
}

Renderpass* RenderpassBuilder::Build(bool concatenateSubpasses)
{
    if(m_numSubpasses < 0) throw std::exception("Cannot create a render pass without any subpasses!");
    m_currentSubpass->EndSubpass();

    if(concatenateSubpasses)
    {
        MachineCode* instructions = new MachineCode[m_totalInstructions];
        Variable* variables = new Variable[m_totalVariables];
        uint32_t numInstructions = 0, numVariables = 0;
        Subpass* current = m_first;
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
        delete m_first;
        current = new Subpass("Concatanated", SubpassFlags::DEFAULT, 0);
        current->Queue = new Renderqueue(instructions, variables, numInstructions, numVariables);
        return new Renderpass(current, 1);
    }
    return new Renderpass(m_first, m_numSubpasses);
}

RenderpassBuilder Renderpass::Create()
{
    return RenderpassBuilder();
}


void Renderpass::Execute()
{
    Subpass* current = m_first;
    while(current != nullptr)
    {
        m_machine->Run(current->Queue);
        current = current->Next;
    } 
}

}