#pragma once

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
class Shader;
class UniformData;

class Material
{
friend class Shader;
friend class Renderer;
friend class Components::MeshComponent;
private:
    Shader* _shader;
    std::vector<UniformData> _uniforms;
    Texture* _texture = nullptr;
    Texture* _normalMap = nullptr;
    std::map<std::string, Texture*> _textures;

    void UpdateTextures();

    Material(Shader* shader);
    
public:

    ~Material();
    
    uint32_t GetUniformCount();

    UniformData* GetUniform(uint32_t index);

    UniformData* GetUniform(std::string name);

    void SetTexture(std::string name, Texture* texture);

    void SetTexture(Texture* texture);

    void SetNormalMap(Texture* normalMap);

    Texture* GetNormalMap();

    Texture* GetTexture();

    void UpdateUniforms();
};


} // namespace Rendering



