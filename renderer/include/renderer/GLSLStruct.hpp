#pragma once

#include "renderer/RenderingTypedefs.hpp"
#include "renderer/RenderingStructs.hpp"

#include <vector>
#include <string>
#include <unordered_map>

namespace Rendering
{



class GLSLStruct
{

protected:
    enum GLSLType { FLOAT, VEC2, VEC3, VEC4, INT, IVEC2, IVEC3, IVEC4, MAT3, MAT4, SAMPLER2D, SAMPLERCUBE };
    struct GLSLVariable { GLSLType Type; std::string Name; };
    struct GLSLStructVar { GLSLStruct* Struct; std::string Name; };
    struct GLSLStructArrVar { GLSLStruct* Struct; std::string Name; ElementCount Count; };

    typedef char Byte;
    typedef std::vector<GLSLVariable> Variables;
    typedef std::vector<GLSLStruct*> Structs;
    typedef std::unordered_map<std::string, GLSLStructArrVar> StructArrays;
    typedef std::unordered_map<std::string, VarOffset> Offsets;
    typedef std::unordered_map<std::string, Byte*> Pointers;

    Variables _vars;
    Structs _structs;
    StructArrays _arrays;
    Offsets _offsets;
    Pointers _pointers;
    Byte* _data = nullptr;
    uint32_t _numInstances = 0;
    GLSLStruct(std::string name, Variables vars, Structs structs, StructArrays arrays, Offsets offsets, StructSize size);
    GLSLStruct(const GLSLStruct& old);
    void InitializePointerTable();
public:
    BufferHandle _uniformBuffer;
    const StructSize Size;
    const std::string Name;
    ~GLSLStruct();
    void Allocate(ElementCount numInstances);
    void Free();
    void UpdateUniformBuffer();
    void BindUniformBuffer(Index instanceIndex, Index bindingIndex);

    template <typename T>
    void SetMember(Index instanceIndex, std::string name, T& value);

    template <typename T>
    T* GetMember(Index instanceIndex, std::string name);

    class StructBuilder
    {
    friend class GLSLStruct;
    private:
        std::string _name;
        // Alignment bins
        std::vector<GLSLVariable> _byte1;
        std::vector<GLSLVariable> _byte2;
        std::vector<GLSLVariable> _byte4;
        std::vector<GLSLStructVar> _structs;
        std::vector<GLSLStructArrVar> _structArrays;
        StructBuilder(std::string name);
    public:
        //TODO: Implement existing variable name check
        StructBuilder& WithFloat(std::string name);
        StructBuilder& WithInt(std::string name);
        StructBuilder& WithVec2(std::string name);
        StructBuilder& WithVec3(std::string name);
        StructBuilder& WithVec4(std::string name);
        StructBuilder& WithIVec2(std::string name);
        StructBuilder& WithIVec3(std::string name);
        StructBuilder& WithIVec4(std::string name);
        StructBuilder& WithMat3(std::string name);
        StructBuilder& WithMat4(std::string name);
        StructBuilder& WithSampler2D(std::string name);
        StructBuilder& WithSamplerCube(std::string name);
        StructBuilder& WithStruct(GLSLStruct* str, std::string name);
        StructBuilder& WithStructArray(GLSLStruct* str, std::string name, ElementCount count);
        GLSLStruct* Build();
    };
    static StructBuilder Create(std::string name);

};


template <typename T>
inline void GLSLStruct::SetMember(Index instanceIndex, std::string name, T& value)
{
    if(instanceIndex >= _numInstances) 
    {
        auto msg = ("Trying to access struct \""+ Name +"\" instance index " + std::to_string(instanceIndex) + ", but has fewer allocated instances! (" + std::to_string(_numInstances) + ")");
        throw new std::exception(msg.c_str());
    }
    if(!_pointers.contains(name))
    {
        auto msg = "Member '" + name + "' does not exist!";
        throw new std::exception(msg.c_str());
    } 
    auto s = sizeof(T);
    if(_offsets[name] + s > Size) throw new std::exception("Failed setting struct member with value that does not fit within the struct size!");
    auto ptr = (T*)_pointers[name] + instanceIndex*Size;
    *ptr = value;
}


template <typename T>
inline T* GLSLStruct::GetMember(Index instanceIndex, std::string name)
{
    if(instanceIndex >= _numInstances)
    {
        auto msg = ("Trying to access struct \""+ Name +"\" instance index " + std::to_string(instanceIndex) + ", but has fewer allocated instances! (" + std::to_string(_numInstances) + ")");
        throw new std::exception(msg.c_str());
    }
    auto ptr = (T*)_pointers[name] + instanceIndex*Size;
    return ptr;
}

} // namespace Rendering
