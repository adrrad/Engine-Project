#pragma once



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

    void Set1fv(std::string name, const float* values, uint32_t count);
};

} // namespace Rendering



