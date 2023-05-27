#pragma once
#include "EngineComponent.hpp"
#include "rendering/GLSLStruct.hpp"

#include <glm/glm.hpp>
#include <glad/glad.h>

#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>

namespace Gameplay 
{
    class MeshComponent;
};

namespace Rendering
{
class Renderer;
typedef GLenum UType;
typedef std::string VarName;

class Shader : public SubsystemComponent<Renderer>
{
friend class Gameplay::MeshComponent;
friend class Renderer;
friend class Material;
friend class Renderpass;
friend class EngineComponent;
private:
    uint32_t ID;
    std::vector<Material*> m_materials;
    std::vector<std::string> m_textures;
    std::unordered_map<std::string, GLSLStruct*> m_uniformBlocks;
    Index m_numInstances = 0;
    ElementCount m_maxInstances = 0;
    std::string m_vertexSource;
    std::string m_fragmentSource;
    std::vector<std::string> m_vFiles;
    std::vector<std::string> m_fFiles;
    std::string m_name;
    
    bool CheckShaderStatus(uint32_t shader, std::string type, bool stopOnFailure = true);
    
    int ULoc(std::string name);

    void CompileShader();

    bool RecompileShader();

    void UpdateUniformBuffers();

    Shader(std::string name, std::string vertexSource, std::string fragmentSource, std::vector<GLSLStruct*> blocks, std::vector<std::string> textures);
public:
    Shader(std::string vertexPath, std::string fragmentPath);
    Shader(std::vector<std::string> vertexPath, std::vector<std::string> fragmentPath);

    ~Shader();

    int GetUniformLocation(std::string name);

    void AllocateBuffers(ElementCount numInstances);

    void Use();

    uint32_t GetProgramID();

    Material* CreateMaterial();

    void SetMat4(std::string name, glm::mat4 mat, uint32_t count);
    
    void SetVec4(std::string name, glm::vec4 value);

    static Shader* GetLineShader();

    class ShaderBuilder : public SubsystemComponent<Renderer>
    {
    friend class Shader;
    private:
        std::string m_name;
        std::string m_header;
        std::string m_vertIO;
        std::string m_vertBlocks;
        std::string m_vertMain;
        std::string m_fragIO;
        std::string m_fragBlocks;
        std::string m_fragMain;
        std::vector<GLSLStruct*> m_uniformBlocks;
        std::vector<std::string> m_textures;

        ShaderBuilder(std::string name);
        ShaderBuilder& WithStandardHeader();
        ShaderBuilder& WithStandardIO();

    public:
        ShaderBuilder& WithWorldSpaceVertexFunctions();
        ShaderBuilder& WithPPVertexFunctions();
        ShaderBuilder& WithGBuffer();
        ShaderBuilder& WithDeferredPBRLighting();
        ShaderBuilder& WithDeferredPBRDirectionalLighting();
        ShaderBuilder& WithStruct(GLSLStruct* str);
        ShaderBuilder& WithUniformStruct(GLSLStruct* str, std::string varname, bool withDefinition);
        ShaderBuilder& WithUniformBlock(GLSLStruct* str, std::string name, bool external = false);
        ShaderBuilder& Shadowmap();
        Shader* Build();
    };
    static ShaderBuilder Create(std::string name);
};

} // namespace Rendering



