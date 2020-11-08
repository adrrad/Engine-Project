#pragma once

#include "rendering/Texture.hpp"
#include "rendering/RenderingTypedefs.hpp"
#include "assets/resources/ImageAsset.hpp"

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
    Shader* m_shader;
    BufferHandle m_vao;
    Index m_instanceIndex;

    std::map<std::string, Texture*> m_textures;

    Material(Shader* shader, Index instanceIndex);

    void CreateVAO(BufferHandle vbo, BufferHandle ebo);
    
public:

    ~Material();

    void SetTexture(std::string name, Texture* texture);

    void UseTextureAsset(std::string name, Assets::ImageAsset* textureAsset);


    template <typename T>
    void SetProperty(std::string blockName, std::string varName, T& value);

    template <typename T>
    T& GetProperty(std::string blockName, std::string varName);

    inline BufferHandle GetVAO();

};

template <typename T>
inline void Material::SetProperty(std::string blockName, std::string varName, T& value)
{
    if(!m_shader->m_uniformBlocks.contains(blockName))
    {
        auto msg = "Block '" + blockName + "' does not exsist!";
        throw std::exception(msg.c_str());
    } 
    m_shader->m_uniformBlocks[blockName]->SetMember<T>(m_instanceIndex, varName, value);
}

template <typename T>
inline T& Material::GetProperty(std::string blockName, std::string varName)
{
    return *m_shader->m_uniformBlocks[blockName]->GetMember<T>(m_instanceIndex, varName);
}

BufferHandle Material::GetVAO()
{
    return m_vao;
}

} // namespace Engine::Rendering



