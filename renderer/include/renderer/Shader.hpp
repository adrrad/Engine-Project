#pragma once
#include <glm/glm.hpp>


#include <cstdint>
#include <string>



namespace Rendering
{

class Shader
{
private:
    uint32_t ID;
    void CheckShaderStatus(uint32_t shader, std::string type);
public:
    Shader(std::string vertexPath, std::string fragmentPath);
    ~Shader();

    void Use();
    uint32_t GetID();

    void SetMat4(std::string name, glm::mat4 mat, uint32_t count);
    void SetFloat(std::string name, float value);
};

} // namespace Rendering



