#include "assets/resources/FragmentShaderAsset.hpp"

namespace Engine::Assets
{

FragmentShaderAsset::FragmentShaderAsset(Platform::IO::File* resourceFile, AssetID id) : Asset(resourceFile, id)
{

}

void FragmentShaderAsset::Load()
{
    if(IsLoaded()) return;
    Array<char> src = ResourceFile->ReadAll();
    m_source = std::string(src.Data(), src.Size);
}

void FragmentShaderAsset::Free()
{
    m_source.clear();
}

bool FragmentShaderAsset::IsLoaded()
{
    return m_source != "";
}

} // namespace Engine::Assets
