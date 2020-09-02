#pragma once

#include "renderer/Material.hpp"
#include "renderer/GLSLStruct.hpp"

#include <glm/glm.hpp>
#include <glad/glad.h>

#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>

namespace Components 
{
    class MeshComponent;
};

namespace Rendering
{

typedef GLenum UType;
typedef std::string VarName;

struct UniformData
{
    uint32_t Type;
    VarName Name;
    union
    {
        int i;
        float f;
        glm::vec2 f2;
        glm::vec3 f3;
        glm::vec4 f4;
    };
};

class Shader
{
friend class Components::MeshComponent;
friend class Renderer;
friend class Material;
friend class Renderpass;
private:
    uint32_t ID;
    std::vector<Material*> _materials;
    std::vector<std::string> _textures;
    std::unordered_map<std::string, GLSLStruct*> _uniformBlocks;
    Index _numInstances = 0;
    ElementCount _maxInstances = 0;
    std::string _vertexSource;
    std::string _fragmentSource;
    std::vector<std::string> _vFiles;
    std::vector<std::string> _fFiles;
    std::string _name;
    
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

    void AllocateBuffers(ElementCount numInstances);

    void Use();

    uint32_t GetID();

    Material* CreateMaterial();

    void SetMat4(std::string name, glm::mat4 mat, uint32_t count);
    
    void SetInt(std::string name, int value);

    void SetFloat(std::string name, float value);

    void SetVec2(std::string name, glm::vec2 value);
    
    void SetVec3(std::string name, glm::vec3 value);
    
    void SetVec4(std::string name, glm::vec4 value);

    float GetFloat(std::string name);

    glm::vec2 GetVec2(std::string name);

    glm::vec3 GetVec3(std::string name);

    glm::vec4 GetVec4(std::string name);

    static Shader* WithStandardIncludes(std::string vertex, std::string fragment);

    static Shader* GetWaveShader();

    static Shader* GetGerstnerWaveShader();

    static Shader* GetGerstnerWaveShader_PBR();

    static Shader* GetPhongShader();

    static Shader* GetPBRShader();

    static Shader* GetTexturesPBRShader();

    static Shader* GetLineShader();

    static Shader* GetSkyboxShader();

    class ShaderBuilder
    {
    friend class Shader;
    private:
        std::string _name;
        std::string _header;
        std::string _vertIO;
        std::string _vertBlocks;
        std::string _vertMain;
        std::string _fragIO;
        std::string _fragBlocks;
        std::string _fragMain;
        std::vector<GLSLStruct*> _uniformBlocks;
        std::vector<std::string> _textures;

        ShaderBuilder(std::string name);
        ShaderBuilder& WithStandardHeader();
        ShaderBuilder& WithStandardStructs();
        ShaderBuilder& WithStandardIO();

    public:
        ShaderBuilder& WithSkyboxVertexFunctions();
        ShaderBuilder& WithStandardVertexFunctions();
        ShaderBuilder& WithWorldSpaceVertexFunctions();
        ShaderBuilder& WithSphericalBillboarding();
        ShaderBuilder& WithUnlitSurface();
        ShaderBuilder& WithPPVertexFunctions();
        ShaderBuilder& WithGBuffer();
        ShaderBuilder& WithDeferredPBRLighting();
        ShaderBuilder& WithPBR();
        ShaderBuilder& WithSkybox(bool postDeferred);
        ShaderBuilder& WithStruct(GLSLStruct* str);
        ShaderBuilder& WithUniformStruct(GLSLStruct* str, std::string varname, bool withDefinition);
        ShaderBuilder& WithUniformBlock(GLSLStruct* str, std::string name);
        // ShaderBuilder& WithTexture(std::string name);
        Shader* Build();
    };
    static ShaderBuilder Create(std::string name);
};

} // namespace Rendering



