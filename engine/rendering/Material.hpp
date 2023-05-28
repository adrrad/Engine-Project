#pragma once

#include "EngineComponent.hpp"
#include "rendering/Texture.hpp"
#include "rendering/Shader.hpp"
#include "rendering/RenderingTypedefs.hpp"
#include "assets/resources/ImageAsset.hpp"

#include <glm/glm.hpp>

#include <cstdint>
#include <string>
#include <vector>
#include <map>

namespace Gameplay
{
    class MeshComponent;
}

namespace Rendering
{
class Shader;
class Renderer;
class Material : public SubsystemComponent<Renderer>
{
friend class EngineComponent;
friend class Renderer;
friend class Renderpass;
friend class Gameplay::MeshComponent;
private:
    Shader* m_shader;
    BufferHandle m_vao;
    u64 m_instanceIndex;

    std::map<std::string, Texture*> m_textures;

    Material(Shader* shader, u64 instanceIndex);

    void CreateVAO(BufferHandle vbo, BufferHandle ebo);
    
public:

    ~Material();

    void SetTexture(std::string name, Texture* texture);

    Texture* GetTexture(std::string name);

    void UseTextureAsset(std::string name, Assets::ImageAsset* textureAsset);

    inline u64 GetInstanceIndex();

    inline Shader* GetShader();

    template <typename T>
    void SetProperty(std::string blockName, std::string varName, T& value);

    template <typename T>
    T& GetProperty(std::string blockName, std::string varName);

    inline BufferHandle GetVAO();

};

inline u64 Material::GetInstanceIndex() 
{
    return m_instanceIndex;
}
inline Shader* Material::GetShader() 
{
    return m_shader;
}


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

} // namespace Rendering



