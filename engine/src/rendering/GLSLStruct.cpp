#include "rendering/GLSLStruct.hpp"

#include "utilities/MathUtils.hpp"

#include "rendering/Debugging.hpp"

#include <glad/glad.h>

#include <iostream>

using namespace Engine::Utilities;

namespace Engine::Rendering
{
typedef GLSLStruct::StructBuilder StructBuilder;

GLSLStruct::GLSLStruct(std::string name, Variables vars, Structs structs, StructArrays arrays, Offsets offsets, StructSize size, std::string glsl, Index bindingIndex) : Size(size), Name(name), BindingIndex(bindingIndex)
{
    m_vars = vars;
    m_structs = structs;
    m_arrays = arrays;
    m_offsets = offsets;
    m_glslCode = glsl;
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
    return new GLSLStruct(*this);
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

void GLSLStruct::Allocate(ElementCount numInstances)
{
    if(m_data == nullptr)
    {
        m_numInstances = numInstances;
        m_data = new Byte[Size*numInstances];
        InitializePointerTable();
        glGenBuffers(1, &m_uniformBuffer);
        glBindBuffer(GL_UNIFORM_BUFFER, m_uniformBuffer);
        glBufferData(GL_UNIFORM_BUFFER, Size*numInstances, m_data, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }
}

void GLSLStruct::Free()
{
    if(m_data != nullptr)
    {
        delete[] m_data;
        glDeleteBuffers(1, &m_uniformBuffer);
    }
}

void GLSLStruct::UpdateUniformBuffer()
{
    if(m_data != nullptr)
    {
        glBindBuffer(GL_UNIFORM_BUFFER, m_uniformBuffer);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, Size*m_numInstances, m_data);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }
    else
    {
        throw new std::exception("Trying to update a struct buffer that was not allocated! (Shader allocation missing?)");
    }
}

void GLSLStruct::BindUniformBuffer(Index instanceIndex, ShaderID shaderID)
{
    if(m_data != nullptr)
    {
        if(BindingIndex == 999) throw std::exception("Uniform buffer binding error. Struct has undefined binding index!");
        if(instanceIndex >= m_numInstances) throw std::exception("Uniform buffer binding failed. The instancee index exceeds the number of allocated instances!");
        glBindBuffer(GL_UNIFORM_BUFFER, m_uniformBuffer);
        UPDATE_CALLINFO2("Uniform block: " + Name);
        uint32_t offset = Size*instanceIndex;
        const char* name = Name.c_str();
        Index blockID = glGetUniformBlockIndex(shaderID, name);
        glUniformBlockBinding(shaderID, blockID, BindingIndex);
        glBindBufferRange(GL_UNIFORM_BUFFER, BindingIndex, m_uniformBuffer, offset, Size);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
        // std::cout << "Binding buffer '" + Name + "' with binding " << BindingIndex << ", instance " << instanceIndex << " with buffer " << m_uniformBuffer << " with shader " << shaderID << std::endl;
    }    
    else
    {
        throw new std::exception("Trying to bind a struct buffer that was not created!");
    }
}

StructBuilder::StructBuilder(std::string name)
{
    _name = name;
}

StructBuilder& StructBuilder::WithBool(std::string name)
{
    _byte1.push_back({BOOL, name});
    return *this;
}

StructBuilder& StructBuilder::WithFloat(std::string name)
{
    _byte1.push_back({FLOAT, name});
    return *this;
}

StructBuilder& StructBuilder::WithSampler2D(std::string name)
{
    _byte1.push_back({SAMPLER2D, name});
    return *this;
}

StructBuilder& StructBuilder::WithSamplerCube(std::string name)
{
    _byte1.push_back({SAMPLERCUBE, name});
    return *this;
}


StructBuilder& StructBuilder::WithInt(std::string name)
{
    _byte1.push_back({INT, name});
    return *this;
}

StructBuilder& StructBuilder::WithVec2(std::string name)
{
    _byte2.push_back({VEC2, name});
    return *this;
}

StructBuilder& StructBuilder::WithVec3(std::string name)
{
    _byte4.push_back({VEC3, name});
    return *this;
}

StructBuilder& StructBuilder::WithVec4(std::string name)
{
    _byte4.push_back({VEC4, name});
    return *this;
}

StructBuilder& StructBuilder::WithIVec2(std::string name)
{
    _byte2.push_back({IVEC2, name});
    return *this;
}

StructBuilder& StructBuilder::WithIVec3(std::string name)
{
    _byte4.push_back({IVEC3, name});
    return *this;
}

StructBuilder& StructBuilder::WithIVec4(std::string name)
{
    _byte4.push_back({IVEC4, name});
    return *this;
}

StructBuilder& StructBuilder::WithMat3(std::string name)
{
    _byte4.push_back({MAT3, name});
    return *this;
}

StructBuilder& StructBuilder::WithMat4(std::string name)
{
    _byte4.push_back({MAT4, name});
    return *this;
}

StructBuilder& StructBuilder::WithStruct(GLSLStruct* str, std::string name)
{
    m_structs.push_back({str, name});
    return *this;
}

StructBuilder& StructBuilder::WithStructArray(GLSLStruct* str, std::string name, ElementCount count)
{
    _structArrays.push_back({str, name, count});
    return *this;
}

GLSLStruct* StructBuilder::Build(bool asUniformBlock, Index bindingIndex)
{
    StructSize size = 0;
    Variables vars;
    Structs structs;
    Offsets offsets;
    StructArrays structArrays;
    std::string glsl;

    for(auto& var : _structArrays)
    {
        GLSLStruct* str = var.Struct;
        std::string name = var.Name;
        ElementCount count = var.Count;

        glsl += str->Name + " " + name + "[" + std::to_string(count) + "];\n";

        structs.push_back(str);
        structArrays[name] = var;
        for(Index i = 0; i < count; i++)
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

    for(GLSLVariable& var : _byte4)
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

    for(GLSLVariable& var : _byte2)
    {
        std::string name = var.Name;
        vars.push_back(var);
        glsl += (var.Type == IVEC2 ? "ivec2 " : "vec2 ") + name + ";\n";
        offsets[var.Name] = RoundUp(size, 8);
        size += 8; //Here we only consider vec2 and ivec2 so 2x4 bytes
    }

    for(GLSLVariable& var : _byte1)
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
    return new GLSLStruct(_name, vars, structs, structArrays, offsets, size, glsl, bindingIndex);
}

GLSLStruct* StructBuilder::Build()
{
    return Build(false);
}

StructBuilder GLSLStruct::Create(std::string name)
{
    return StructBuilder(name);
}

std::shared_ptr<Utilities::JSON::JSONValue> GLSLStruct::Serialise()
{
    using namespace Utilities::JSON;
    std::vector<std::shared_ptr<JSONKeyValuePair>> members;

    return JSONValue::AsObject({});
}

void GLSLStruct::Deserialise(std::shared_ptr<Utilities::JSON::JSONValue> json)
{
    using namespace Utilities::JSON;
}

} // namespace Engine::Rendering
