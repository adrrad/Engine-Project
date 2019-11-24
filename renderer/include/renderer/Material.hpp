#pragma once
#include "renderer/Shader.hpp"

#include <glm/glm.hpp>


#include <cstdint>
#include <string>
#include <vector>

namespace Components
{
    class MeshComponent;
}

namespace Rendering
{

class Material
{
friend class Renderer;
friend class Components::MeshComponent;
private:
    Shader* _shader;
    std::vector<UniformData> _uniforms;

public:
    Material(Shader* shader);
    ~Material();
    
    uint32_t GetUniformCount();

    UniformData* GetUniform(uint32_t index);

    UniformData* GetUniform(std::string name);

    void UpdateUniforms();
};

} // namespace Rendering



