#pragma once

#include "EngineComponent.hpp"
#include "EngineTypedefs.hpp"
#include "Serialisable.hpp"
#include "rendering/RenderingTypedefs.hpp"
#include "rendering/RenderingStructs.hpp"

#include <vector>
#include <string>
#include <unordered_map>

namespace Rendering
{

class Renderer;

class GLSLStruct : public SubsystemComponent<Renderer>, public Serialisable 
{
    static std::unordered_map<std::string, GLSLStruct*> ALL_STRUCTS;
protected:
    enum GLSLType { BOOL, FLOAT, VEC2, VEC3, VEC4, INT, IVEC2, IVEC3, IVEC4, MAT3, MAT4, SAMPLER2D, SAMPLERCUBE };
    struct GLSLVariable { GLSLType Type; std::string Name; };
    struct GLSLStructVar { GLSLStruct* Struct; std::string Name; };
    struct GLSLStructArrVar { GLSLStruct* Struct; std::string Name; u64 Count; };

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
    Buffer m_buffer;

    
    
    void InitializePointerTable();
public:
    StructSize Size;
    std::string Name;
    u64 BindingIndex;

    GLSLStruct();
    
    GLSLStruct(std::string name, Variables vars, Structs structs, StructArrays arrays, Offsets offsets, StructSize size, std::string glsl, u64 bindingIndex);

    GLSLStruct(const GLSLStruct& old);
    
    ~GLSLStruct();
    GLSLStruct* GetCopy();
    std::string GetGLSLCode(bool isUniform, bool isBlock, std::string varName = "");
    void Allocate(uint32_t numInstances);
    void Free();
    void UpdateUniformBuffer();
    void BindUniformBuffer(u64 instanceIndex, ShaderID shaderID);

    /**
     * @brief Gets and already created struct by its name.
     * 
     * @param name The name of the struct.
     * @return GLSLStruct* Pointer to the struct, nullptr if no struct of the given name exist.
     */
    static inline GLSLStruct* Get(std::string name)
    {
        if(ALL_STRUCTS.contains(name)) return ALL_STRUCTS[name];
        return nullptr;
    }
    
    inline BufferHandle GetUniformBuffer()
    {
        return m_buffer.Handle;
    }

    inline VarOffset GetInstanceOffset(u64 instanceIndex)
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
    void SetMember(u64 instanceIndex, std::string name, T& value);

    template <typename T>
    T* GetMember(u64 instanceIndex, std::string name);

    class StructBuilder : public SubsystemComponent<Renderer>
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
        StructBuilder& WithStructArray(GLSLStruct* str, std::string name, u64 count);
        GLSLStruct* Build(bool asUniformBlock, u64 bindingIndex = 999);
        GLSLStruct* Build();
    };
    static StructBuilder Create(std::string name);

    static GLSLStruct* GetDeserialised(std::shared_ptr<Utilities::JSON::JSONValue> json);

    std::shared_ptr<Utilities::JSON::JSONValue> Serialise() override;

    void Deserialise(std::shared_ptr<Utilities::JSON::JSONValue> json) override;

};


template <typename T>
inline void GLSLStruct::SetMember(u64 instanceIndex, std::string name, T& value)
{
    Assert(instanceIndex < m_numInstances, "Instance index out of range!");
    Assert(m_pointers.contains(name), "Member with the given name does not exist!");
    auto s = sizeof(T);
    Assert(m_offsets[name] + s <= Size, "Value size getting out of struct range!");
    auto ptr = (T*)(m_pointers[name] + instanceIndex*Size);
    *ptr = value;
}


template <typename T>
inline T* GLSLStruct::GetMember(u64 instanceIndex, std::string name)
{
    Assert(instanceIndex < m_numInstances, "Instance index out of range!");
    Assert(m_pointers.contains(name), "Member with the given name does not exist!");
    auto ptr = (T*)(m_pointers[name] + instanceIndex*Size);
    return ptr;
}

} // namespace Rendering
