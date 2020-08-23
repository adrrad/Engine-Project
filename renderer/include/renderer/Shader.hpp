#pragma once

#include "renderer/Material.hpp"

#include <glm/glm.hpp>
#include <glad/glad.h>

#include <cstdint>
#include <string>
#include <vector>

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
private:
    uint32_t ID;
    std::vector<Material*> _materials;
    std::vector<UniformData> _activeUniforms;
    std::string _vertexSource;
    std::string _fragmentSource;
    std::vector<std::string> _vFiles;
    std::vector<std::string> _fFiles;
    
    bool CheckShaderStatus(uint32_t shader, std::string type, bool stopOnFailure = true);
    
    int ULoc(std::string name);

    void CompileShader();

    bool RecompileShader();

public:
    Shader(std::string vertexPath, std::string fragmentPath);
    Shader(std::vector<std::string> vertexPath, std::vector<std::string> fragmentPath);
    ~Shader();

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

    std::vector<UniformData> GetActiveUniforms();

    static Shader* WithStandardIncludes(std::string vertex, std::string fragment);

    static Shader* GetWaveShader();

    static Shader* GetGerstnerWaveShader();

    static Shader* GetGerstnerWaveShader_PBR();

    static Shader* GetPhongShader();

    static Shader* GetPBRShader();

    static Shader* GetTexturesPBRShader();

    static Shader* GetLineShader();

    static Shader* GetSkyboxShader();

};

} // namespace Rendering



