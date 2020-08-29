#pragma once

#include "renderer/Texture.hpp"
#include "renderer/RenderingTypedefs.hpp"
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
friend class Renderpass;
friend class Components::MeshComponent;
private:
    Shader* _shader;
    Index _instanceIndex;
    std::map<std::string, Texture*> _textures;

    Material(Shader* shader, Index instanceIndex);
    
    void UpdateUniforms();
public:

    ~Material();

    void SetTexture(std::string name, Texture* texture);


    template <typename T>
    void SetProperty(std::string blockName, std::string varName, T& value);

    template <typename T>
    T& GetProperty(std::string blockName, std::string varName);

};

template <typename T>
inline void Material::SetProperty(std::string blockName, std::string varName, T& value)
{
    if(!_shader->_uniformBlocks.contains(blockName))
    {
        auto msg = "Block '" + blockName + "' does not exsist!";
        throw std::exception(msg.c_str());
    } 
    _shader->_uniformBlocks[blockName]->SetMember<T>(_instanceIndex, varName, value);
}

template <typename T>
inline T& Material::GetProperty(std::string blockName, std::string varName)
{
    return *_shader->_uniformBlocks[blockName]->GetMember<T>(_instanceIndex, varName);
}

} // namespace Rendering



