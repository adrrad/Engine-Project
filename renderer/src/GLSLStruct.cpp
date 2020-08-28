#include "renderer/GLSLStruct.hpp"

#include "utilities/MathUtils.hpp"

#include <glad/glad.h>

using namespace Utilities;

namespace Rendering
{
typedef GLSLStruct::StructBuilder StructBuilder;

GLSLStruct::GLSLStruct(std::string name, Variables vars, Structs structs, StructArrays arrays, Offsets offsets, StructSize size) : Size(size), Name(name)
{
    _vars = vars;
    _structs = structs;
    _arrays = arrays;
    _offsets = offsets;
}

GLSLStruct::GLSLStruct(const GLSLStruct& old) : Size(old.Size)
{
    _vars = old._vars;
    _structs = old._structs;
    _arrays = old._arrays;
    _offsets = old._offsets;
    _data = old._data;
    _pointers = old._pointers;
}

void GLSLStruct::InitializePointerTable()
{
    for(auto& pair : _offsets)
    {
        std::string name = pair.first;
        VarOffset offset = pair.second;
        _pointers[name] = _data + offset;
    }
}

GLSLStruct::~GLSLStruct()
{
    Free();
}

void GLSLStruct::Allocate(ElementCount numInstances)
{
    if(_data == nullptr)
    {
        _numInstances = numInstances;
        _data = new Byte[Size*numInstances];
        InitializePointerTable();
        glGenBuffers(1, &_uniformBuffer);
        glBindBuffer(GL_UNIFORM_BUFFER, _uniformBuffer);
        glBufferData(GL_UNIFORM_BUFFER, Size*numInstances, _data, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }
}

void GLSLStruct::Free()
{
    if(_data != nullptr)
    {
        delete[] _data;
        glDeleteBuffers(1, &_uniformBuffer);
    }
}

void GLSLStruct::UpdateUniformBuffer()
{
    if(_data != nullptr)
    {
        glBindBuffer(GL_UNIFORM_BUFFER, _uniformBuffer);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, Size*_numInstances, _data);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }
    else
    {
        throw new std::exception("Trying to update a struct buffer that was not created!");
    }
    
}

void GLSLStruct::BindUniformBuffer(Index instanceIndex, Index bindingIndex)
{
    if(_data != nullptr)
    {
        if(instanceIndex >= _numInstances) throw new std::exception("Uniform buffer binding failed. The instancee index exceeds the number of allocated instances!");
        glBindBuffer(GL_UNIFORM_BUFFER, _uniformBuffer);
        glBindBufferRange(GL_UNIFORM_BUFFER, bindingIndex, _uniformBuffer, Size*instanceIndex, Size);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
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

StructBuilder& StructBuilder::WithFloat(std::string name)
{
    _byte1.push_back({FLOAT, name});
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

StructBuilder& StructBuilder::WithStruct(GLSLStruct* str, std::string name)
{
    _structs.push_back({str, name});
    return *this;
}

StructBuilder& StructBuilder::WithStructArray(GLSLStruct* str, std::string name, ElementCount count)
{
    _structArrays.push_back({str, name, count});
    return *this;
}

GLSLStruct* StructBuilder::Build()
{
    StructSize size = 0;
    Variables vars;
    Structs structs;
    Offsets offsets;
    StructArrays structArrays;

    for(auto& var : _structArrays)
    {
        GLSLStruct* str = var.Struct;
        std::string name = var.Name;
        ElementCount count = var.Count;
        structs.push_back(str);
        structArrays[name] = var;
        for(Index i = 0; i < count; i++)
        {
            VarOffset offset = RoundUp(size, 4);
            std::string indexName = name+"["+std::to_string(i)+"]";
            offsets[indexName] = offset;
            for(auto& pair : str->_offsets)
            {
                std::string name = pair.first;
                VarOffset mOffset = pair.second;
                offsets[indexName+"."+name] = offset + mOffset;
            }
            size += str->Size;
        }
    }

    for(auto& var : _structs)
    {
        GLSLStruct* str = var.Struct;
        std::string name = var.Name;
        structs.push_back(str);
        VarOffset offset = RoundUp(size, 4);
        offsets[name] = offset;
        for(auto& pair : str->_offsets)
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
        offsets[var.Name] = offset;
        GLSLType t = var.Type;
        switch (t)
        {
        case MAT3:
            size = offset + 36;
            break;
        case MAT4:
            size = offset + 64;
            break;
        case VEC4:
        case IVEC4:
            size = offset + 16;
            break;
        case VEC3:
        case IVEC3:
            size = offset + 12;
            break;
        default:
            break;
        }
    }

    for(GLSLVariable& var : _byte2)
    {
        vars.push_back(var);
        offsets[var.Name] = RoundUp(size, 8);
        size += 8; //Here we only consider vec2 and ivec2 so 2x4 bytes
    }

    for(GLSLVariable& var : _byte1)
    {
        vars.push_back(var);
        offsets[var.Name] = RoundUp(size, 4);
        size += 4;
    }
    size = RoundUp(size, 16); //Struct size is padded to a multiple of the size of vec4
    return new GLSLStruct(_name, vars, structs, structArrays, offsets, size);
}

StructBuilder GLSLStruct::Create(std::string name)
{
    return StructBuilder(name);
}

} // namespace Rendering
