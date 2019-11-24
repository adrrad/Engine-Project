#pragma once
#include <glm/glm.hpp>


#include <cstdint>
#include <string>
#include <vector>


namespace Rendering
{

typedef uint32_t UType;
typedef std::string VarName;

struct UniformData
{
    uint32_t Type;
    VarName Name;
    union
    {
        float f;
        glm::vec2 f2;
        glm::vec3 f3;
        glm::vec4 f4;
    };
};

class Shader
{
private:
    uint32_t ID;
    void CheckShaderStatus(uint32_t shader, std::string type);
    int ULoc(std::string name);
public:
    Shader(std::string vertexPath, std::string fragmentPath);
    ~Shader();

    void Use();
    uint32_t GetID();

    void SetMat4(std::string name, glm::mat4 mat, uint32_t count);
    
    void SetFloat(std::string name, float value);

    void SetVec2(std::string name, glm::vec2 value);
    
    void SetVec3(std::string name, glm::vec3 value);
    
    void SetVec4(std::string name, glm::vec4 value);

    float GetFloat(std::string name);

    glm::vec2 GetVec2(std::string name);

    glm::vec3 GetVec3(std::string name);

    glm::vec4 GetVec4(std::string name);

    std::vector<UniformData> GetActiveUniforms();

    static Shader* GetWaveShader();

    static Shader* GetGerstnerWaveShader();

    static Shader* GetPhongShader();

    static Shader* GetLineShader();

};

} // namespace Rendering



