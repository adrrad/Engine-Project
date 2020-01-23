#pragma once
#include "renderer/Shader.hpp"
#include "renderer/Texture.hpp"

#include <glm/glm.hpp>


#include <cstdint>
#include <string>
#include <vector>
#include <map>

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
    Texture* _texture = nullptr;
public:
    Material(Shader* shader);

    ~Material();
    
    uint32_t GetUniformCount();

    UniformData* GetUniform(uint32_t index);

    UniformData* GetUniform(std::string name);

    void SetTexture(Texture* texture);

    Texture* GetTexture();

    void UpdateUniforms();
};

} // namespace Rendering



