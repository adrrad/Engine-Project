#include "rendering/GLSLStruct.hpp"

#include "utilities/MathUtils.hpp"

#include "rendering/Debugging.hpp"
#include "rendering/Renderer.hpp"

#include <glad/glad.h>

#include <iostream>

using namespace Utilities;
using namespace Memory;
namespace Rendering
{
typedef GLSLStruct::StructBuilder StructBuilder;

std::unordered_map<std::string, GLSLStruct*> GLSLStruct::ALL_STRUCTS;

GLSLStruct::GLSLStruct()
{

}

GLSLStruct::GLSLStruct(std::string name, Variables vars, Structs structs, StructArrays arrays, Offsets offsets, StructSize size, std::string glsl, u64 bindingIndex) : Size(size), Name(name), BindingIndex(bindingIndex)
{
    m_vars = vars;
    m_structs = structs;
    m_arrays = arrays;
    m_offsets = offsets;
    m_glslCode = glsl;
    ALL_STRUCTS.insert({ name, this });
}

GLSLStruct::GLSLStruct(const GLSLStruct& old) : Size(old.Size), Name(old.Name), BindingIndex(old.BindingIndex)
{
    m_vars = old.m_vars;
    m_structs = old.m_structs;
    m_arrays = old.m_arrays;
    m_offsets = old.m_offsets;
    m_data = old.m_data;
    m_pointers = old.m_pointers;
    m_glslCode = old.m_glslCode;
}

void GLSLStruct::InitializePointerTable()
{
    for(auto& pair : m_offsets)
    {
        std::string name = pair.first;
        VarOffset offset = pair.second;
        m_pointers[name] = m_data + offset;
    }
}

GLSLStruct::~GLSLStruct()
{
    Free();
}

GLSLStruct* GLSLStruct::GetCopy()
{
    return New<GLSLStruct>(*this);
}

std::string GLSLStruct::GetGLSLCode(bool isUniform, bool isBlock, std::string varName)
{
    std::string binding = BindingIndex == 999 ? "" : ", binding=" + std::to_string(BindingIndex);
    std::string ret = isBlock && isUniform ? "layout(std140" + binding + ") " : "";
    ret += isUniform ? "uniform " : "";
    ret += !isBlock ? "struct " : "";
    ret += Name + "\n{\n";
    ret += m_glslCode;
    ret += "}" + (varName == "" ? "" : " " + varName) + ";\n";
    return ret;
}

void GLSLStruct::Allocate(uint32_t numInstances)
{
    if(m_data == nullptr)
    {
        uint32_t bufferSize = Size*numInstances;
        m_numInstances = numInstances;
        m_data = new Byte[bufferSize];
        InitializePointerTable();
        Renderer::GetInstance()->CreateUniformBuffer(m_buffer, bufferSize, m_data);
    }
}

void GLSLStruct::Free()
{
    if(m_data != nullptr)
    {
        delete[] m_data;
        Renderer::GetInstance()->FreeUniformBuffer(m_buffer);
    }
}

void GLSLStruct::UpdateUniformBuffer()
{
    Assert(m_data != nullptr, "Trying to update a struct buffer that was not allocated!");
    glBindBuffer(GL_UNIFORM_BUFFER, m_buffer.Handle);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, Size*m_numInstances, m_data);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void GLSLStruct::BindUniformBuffer(u64 instanceIndex, ShaderID shaderID)
{
    Assert(m_data != nullptr, "Trying to bind a struct buffer that was not created!");
    Assert(BindingIndex != 999, "Uniform buffer binding error. Struct has undefined binding index!");
    Assert(instanceIndex < m_numInstances, "Uniform buffer binding failed. The instancee index exceeds the number of allocated instances!");
    glBindBuffer(GL_UNIFORM_BUFFER, m_buffer.Handle);
    UPDATE_CALLINFO2("Uniform block: " + Name);
    uint32_t offset = Size*instanceIndex;
    const char* name = Name.c_str();
    u64 blockID = glGetUniformBlockIndex(shaderID, name);
    glUniformBlockBinding(shaderID, blockID, BindingIndex);
    glBindBufferRange(GL_UNIFORM_BUFFER, BindingIndex, m_buffer.Handle, offset, Size);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

StructBuilder::StructBuilder(std::string name)
{
    m_name = name;
}

StructBuilder& StructBuilder::WithBool(std::string name)
{
    m_byte1.push_back({BOOL, name});
    return *this;
}

StructBuilder& StructBuilder::WithFloat(std::string name)
{
    m_byte1.push_back({FLOAT, name});
    return *this;
}

StructBuilder& StructBuilder::WithSampler2D(std::string name)
{
    m_byte1.push_back({SAMPLER2D, name});
    return *this;
}

StructBuilder& StructBuilder::WithSamplerCube(std::string name)
{
    m_byte1.push_back({SAMPLERCUBE, name});
    return *this;
}


StructBuilder& StructBuilder::WithInt(std::string name)
{
    m_byte1.push_back({INT, name});
    return *this;
}

StructBuilder& StructBuilder::WithVec2(std::string name)
{
    m_byte2.push_back({VEC2, name});
    return *this;
}

StructBuilder& StructBuilder::WithVec3(std::string name)
{
    m_byte4.push_back({VEC3, name});
    return *this;
}

StructBuilder& StructBuilder::WithVec4(std::string name)
{
    m_byte4.push_back({VEC4, name});
    return *this;
}

StructBuilder& StructBuilder::WithIVec2(std::string name)
{
    m_byte2.push_back({IVEC2, name});
    return *this;
}

StructBuilder& StructBuilder::WithIVec3(std::string name)
{
    m_byte4.push_back({IVEC3, name});
    return *this;
}

StructBuilder& StructBuilder::WithIVec4(std::string name)
{
    m_byte4.push_back({IVEC4, name});
    return *this;
}

StructBuilder& StructBuilder::WithMat3(std::string name)
{
    m_byte4.push_back({MAT3, name});
    return *this;
}

StructBuilder& StructBuilder::WithMat4(std::string name)
{
    m_byte4.push_back({MAT4, name});
    return *this;
}

StructBuilder& StructBuilder::WithStruct(GLSLStruct* str, std::string name)
{
    m_structs.push_back({str, name});
    return *this;
}

StructBuilder& StructBuilder::WithStructArray(GLSLStruct* str, std::string name, u64 count)
{
    m_structArrays.push_back({str, name, count});
    return *this;
}

GLSLStruct* StructBuilder::Build(bool asUniformBlock, u64 bindingIndex)
{
    StructSize size = 0;
    Variables vars;
    Structs structs;
    Offsets offsets;
    StructArrays structArrays;
    std::string glsl;

    for(auto& var : m_structArrays)
    {
        GLSLStruct* str = var.Struct;
        std::string name = var.Name;
        u64 count = var.Count;

        glsl += str->Name + " " + name + "[" + std::to_string(count) + "];\n";

        structs.push_back(str);
        structArrays[name] = var;
        for(u64 i = 0; i < count; i++)
        {
            VarOffset offset = RoundUp(size, 16);
            std::string indexName = name+"["+std::to_string(i)+"]";
            offsets[indexName] = offset;
            for(auto& pair : str->m_offsets)
            {
                std::string name = pair.first;
                VarOffset mOffset = pair.second;
                offsets[indexName+"."+name] = offset + mOffset;
            }
            size += str->Size;
        }
    }

    for(auto& var : m_structs)
    {
        GLSLStruct* str = var.Struct;
        std::string name = var.Name;

        glsl += str->Name + " " + name + ";\n";

        structs.push_back(str);
        VarOffset offset = RoundUp(size, 16);
        offsets[name] = offset;
        for(auto& pair : str->m_offsets)
        {
            std::string name = pair.first;
            VarOffset mOffset = pair.second;
            offsets[name+"."+name] = offset + mOffset;
        }
        size += str->Size;
    }

    for(GLSLVariable& var : m_byte4)
    {
        vars.push_back(var);
        VarOffset offset = RoundUp(size, 16);
        std::string name = var.Name;
        offsets[name] = offset;
        GLSLType t = var.Type;
        switch (t)
        {
        case MAT3:
            size = offset + 36;
            glsl += "mat3 " + name + ";\n";
            break;
        case MAT4:
            size = offset + 64;
            glsl += "mat4 " + name + ";\n";
            break;
        case VEC4:
            glsl += "vec4 " + name + ";\n";
            size = offset + 16;
            break;
        case IVEC4:
            glsl += "ivec4 " + name + ";\n";
            size = offset + 16;
            break;
        case VEC3:
            glsl += "vec3 " + name + ";\n";
            size = offset + 12;
            break;
        case IVEC3:
            glsl += "ivec3 " + name + ";\n";
            size = offset + 12;
            break;
        default:
            break;
        }
    }

    for(GLSLVariable& var : m_byte2)
    {
        std::string name = var.Name;
        vars.push_back(var);
        glsl += (var.Type == IVEC2 ? "ivec2 " : "vec2 ") + name + ";\n";
        offsets[var.Name] = RoundUp(size, 8);
        size += 8; //Here we only consider vec2 and ivec2 so 2x4 bytes
    }

    for(GLSLVariable& var : m_byte1)
    {
        std::string name = var.Name;
        vars.push_back(var);
        
        if(var.Type == FLOAT) glsl += "float " + name + ";\n";
        else if(var.Type == INT) glsl += "int " + name + ";\n";
        else if(var.Type == BOOL) glsl += "bool " + name + ";\n";
        else if(var.Type == SAMPLER2D) glsl += "sampler2D " + name + ";\n";
        else if(var.Type == SAMPLERCUBE) glsl += "samplerCube " + name + ";\n";
        else throw std::exception("Unkown GLSL struct datatype");
        offsets[var.Name] = RoundUp(size, 4);
        size += 4;
    }
    int alignment = 0;
    glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &alignment);
    if(asUniformBlock) size = RoundUp(size, alignment); //Uniform block must be aligned according to the implementation requirements 
    else size = RoundUp(size, 16); //Struct size is padded to a multiple of the size of vec4
    return New<GLSLStruct>(m_name, vars, structs, structArrays, offsets, size, glsl, bindingIndex);
}

GLSLStruct* StructBuilder::Build()
{
    return Build(false);
}

StructBuilder GLSLStruct::Create(std::string name)
{
    return StructBuilder(name);
}

GLSLStruct* GLSLStruct::GetDeserialised(std::shared_ptr<Utilities::JSON::JSONValue> json)
{
    Allocator* allocator = EngineSubsystem<Renderer>::GetSubsystemAllocator();
    GLSLStruct* str = allocator->New<GLSLStruct>();
    str->Deserialise(json);
    return str;
}

std::shared_ptr<Utilities::JSON::JSONValue> GLSLStruct::Serialise()
{
    using namespace Utilities::JSON;
    std::vector<JSONKeyValuePair> properties;
    std::vector<JSONKeyValuePair> members;
    for(auto var : m_offsets)
    {
        std::string name = var.first;
        u32 offset = var.second;
        members.push_back({name, JSONValue::AsFloat(offset)});
    }
    properties.push_back({ "size", JSONValue::AsFloat(Size)});
    properties.push_back({ "name", JSONValue::AsString(Name)});
    properties.push_back({ "bindingIndex", JSONValue::AsFloat(BindingIndex)});
    properties.push_back({ "code", JSONValue::AsString(m_glslCode)});
    properties.push_back({ "members", JSONValue::AsObject(members)});
    return JSONValue::AsObject(properties);
}

void GLSLStruct::Deserialise(std::shared_ptr<Utilities::JSON::JSONValue> json)
{
    using namespace Utilities::JSON;
    auto offsets = json->GetMember("members");
    Size = json->GetMember("size")->Float;
    Name = json->GetMember("name")->String;
    BindingIndex = json->GetMember("bindingIndex")->Float;
    m_glslCode = json->GetMember("code")->String;
    for(auto member : offsets->Members)
    {
        std::string name = member.Key;
        u32 offset = member.Value->Float;
        m_offsets[name] = offset;
    }
    InitializePointerTable();
}

} // namespace Rendering
