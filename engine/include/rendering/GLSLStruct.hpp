#pragma once

#include "EngineTypedefs.hpp"
#include "Serialisable.hpp"
#include "rendering/RenderingTypedefs.hpp"
#include "rendering/RenderingStructs.hpp"

#include <vector>
#include <string>
#include <unordered_map>

namespace Engine::Rendering
{



class GLSLStruct : public Serialisable
{

protected:
    enum GLSLType { BOOL, FLOAT, VEC2, VEC3, VEC4, INT, IVEC2, IVEC3, IVEC4, MAT3, MAT4, SAMPLER2D, SAMPLERCUBE };
    struct GLSLVariable { GLSLType Type; std::string Name; };
    struct GLSLStructVar { GLSLStruct* Struct; std::string Name; };
    struct GLSLStructArrVar { GLSLStruct* Struct; std::string Name; ElementCount Count; };

    typedef char Byte;
    typedef std::vector<GLSLVariable> Variables;
    typedef std::vector<GLSLStruct*> Structs;
    typedef std::unordered_map<std::string, GLSLStructArrVar> StructArrays;
    typedef std::unordered_map<std::string, VarOffset> Offsets;
    typedef std::unordered_map<std::string, Byte*> Pointers;

    Variables m_vars;
    Structs m_structs;
    StructArrays m_arrays;
    Offsets m_offsets;
    Pointers m_pointers;
    Byte* m_data = nullptr;
    uint32_t m_numInstances = 0;
    std::string m_glslCode = ""; //TODO: Do not store the code string but generate it in the corresponding getter.
    BufferHandle m_uniformBuffer;
    
    GLSLStruct();
    
    GLSLStruct(std::string name, Variables vars, Structs structs, StructArrays arrays, Offsets offsets, StructSize size, std::string glsl, Index bindingIndex);

    GLSLStruct(const GLSLStruct& old);
    
    void InitializePointerTable();
public:
    StructSize Size;
    std::string Name;
    Index BindingIndex;
    ~GLSLStruct();
    GLSLStruct* GetCopy();
    std::string GetGLSLCode(bool isUniform, bool isBlock, std::string varName = "");
    void Allocate(ElementCount numInstances);
    void Free();
    void UpdateUniformBuffer();
    void BindUniformBuffer(Index instanceIndex, ShaderID shaderID);
    
    inline BufferHandle GetUniformBuffer()
    {
        return m_uniformBuffer;
    }

    inline VarOffset GetInstanceOffset(Index instanceIndex)
    {
        return instanceIndex*Size;
    }

    /**
     * @brief Returns the number of allocated instances
     * 
     * @return uint32_t 
     */
    inline uint32_t GetInstancesCount()
    {
        return m_numInstances;
    }

    template <typename T>
    void SetMember(Index instanceIndex, std::string name, T& value);

    template <typename T>
    T* GetMember(Index instanceIndex, std::string name);

    class StructBuilder
    {
    friend class GLSLStruct;
    private:
        std::string m_name;
        // Alignment bins
        std::vector<GLSLVariable> m_byte1;
        std::vector<GLSLVariable> m_byte2;
        std::vector<GLSLVariable> m_byte4;
        std::vector<GLSLStructVar> m_structs;
        std::vector<GLSLStructArrVar> m_structArrays;
        StructBuilder(std::string name);
    public:
        //TODO: Implement existing variable name check
        StructBuilder& WithBool(std::string name);
        StructBuilder& WithFloat(std::string name);
        StructBuilder& WithSampler2D(std::string name);
        StructBuilder& WithSamplerCube(std::string name);
        StructBuilder& WithInt(std::string name);
        StructBuilder& WithVec2(std::string name);
        StructBuilder& WithVec3(std::string name);
        StructBuilder& WithVec4(std::string name);
        StructBuilder& WithIVec2(std::string name);
        StructBuilder& WithIVec3(std::string name);
        StructBuilder& WithIVec4(std::string name);
        StructBuilder& WithMat3(std::string name);
        StructBuilder& WithMat4(std::string name);
        StructBuilder& WithStruct(GLSLStruct* str, std::string name);
        StructBuilder& WithStructArray(GLSLStruct* str, std::string name, ElementCount count);
        GLSLStruct* Build(bool asUniformBlock, Index bindingIndex = 999);
        GLSLStruct* Build();
    };
    static StructBuilder Create(std::string name);

    static GLSLStruct* GetDeserialised(std::shared_ptr<Utilities::JSON::JSONValue> json);

    std::shared_ptr<Utilities::JSON::JSONValue> Serialise() override;

    void Deserialise(std::shared_ptr<Utilities::JSON::JSONValue> json) override;

};


template <typename T>
inline void GLSLStruct::SetMember(Index instanceIndex, std::string name, T& value)
{
    if(instanceIndex >= m_numInstances) 
    {
        auto msg = ("Trying to access struct \""+ Name +"\" instance index " + std::to_string(instanceIndex) + ", but has fewer allocated instances! (" + std::to_string(m_numInstances) + ")");
        throw new std::exception(msg.c_str());
    }
    if(!m_pointers.contains(name))
    {
        auto msg = "Member '" + name + "' does not exist!";
        throw new std::exception(msg.c_str());
    } 
    auto s = sizeof(T);
    if(m_offsets[name] + s > Size) throw new std::exception("Failed setting struct member with value that does not fit within the struct size!");
    auto ptr = (T*)(m_pointers[name] + instanceIndex*Size);
    *ptr = value;
}


template <typename T>
inline T* GLSLStruct::GetMember(Index instanceIndex, std::string name)
{
    if(instanceIndex >= m_numInstances)
    {
        auto msg = ("Trying to access struct \""+ Name +"\" instance index " + std::to_string(instanceIndex) + ", but has fewer allocated instances! (" + std::to_string(m_numInstances) + ")");
        throw new std::exception(msg.c_str());
    }
    auto ptr = (T*)(m_pointers[name] + instanceIndex*Size);
    return ptr;
}

} // namespace Engine::Rendering
