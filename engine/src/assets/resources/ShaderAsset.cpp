#pragma once
#include "assets/resources/ShaderAsset.hpp"
#include "assets/resources/FragmentShaderAsset.hpp"
#include "assets/resources/VertexShaderAsset.hpp"
#include "assets/AssetManager.hpp"
namespace Engine::Assets
{

ShaderAsset::ShaderAsset(Platform::IO::File* resourceFile, AssetID id) : Asset(resourceFile, id)
{

}

void ShaderAsset::Load()
{
    if(IsLoaded()) return;
    AssetManager* assetManager = AssetManager::GetInstance();
    VertexShaderAsset* vert = dynamic_cast<VertexShaderAsset*>(assetManager->GetAsset(m_vertexShaderID));
    FragmentShaderAsset* frag = dynamic_cast<FragmentShaderAsset*>(assetManager->GetAsset(m_fragmentShaderID));
    if(!vert) throw EngineException("Shader asset loading error: Vertex shader asset '" + m_vertexShaderID.ToString() + "' does not exist!");
    if(!frag) throw EngineException("Shader asset loading error: Fragment shader asset '" + m_fragmentShaderID.ToString() + "' does not exist!");
    m_shaderData = new ShaderData(vert->GetSource(), frag->GetSource());
}

void ShaderAsset::Free()
{
    if(!IsLoaded()) return;
    delete m_shaderData;
    m_shaderData = nullptr;
}

bool ShaderAsset::IsLoaded()
{
    return m_shaderData != nullptr;
}

} // namespace Engine::Assets
