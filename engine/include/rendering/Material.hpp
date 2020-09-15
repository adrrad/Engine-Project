#pragma once

#include "rendering/Texture.hpp"
#include "rendering/RenderingTypedefs.hpp"
#include <glm/glm.hpp>


#include <cstdint>
#include <string>
#include <vector>
#include <map>

namespace Engine::Components
{
    class MeshComponent;
}

namespace Engine::Rendering
{
class Shader;

class Material
{
friend class Shader;
friend class Renderer;
friend class Renderpass;
friend class Components::MeshComponent;
private:
    Shader* _shader;
    BufferHandle _vao;
    Index _instanceIndex;

    std::map<std::string, Texture*> _textures;

    Material(Shader* shader, Index instanceIndex);

    void CreateVAO(BufferHandle vbo, BufferHandle ebo);
    
public:

    ~Material();

    void SetTexture(std::string name, Texture* texture);


    template <typename T>
    void SetProperty(std::string blockName, std::string varName, T& value);

    template <typename T>
    T& GetProperty(std::string blockName, std::string varName);

    inline BufferHandle GetVAO();

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

BufferHandle Material::GetVAO()
{
    return _vao;
}

} // namespace Engine::Rendering



