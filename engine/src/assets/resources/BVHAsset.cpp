#include "assets/resources/BVHAsset.hpp"

namespace Engine::Assets
{

BVHAsset::BVHAsset(Platform::IO::File* resourceFile, AssetID id) : Asset(resourceFile, id)
{

}

void BVHAsset::Load()
{
    if(IsLoaded()) return;
    Array<char> src = ResourceFile->ReadAll();
    std::string content = std::string(src.Data(), src.Size);
    m_animation = new Utilities::BVH::BVHAnimation(content);

}

void BVHAsset::Free()
{
    delete m_animation;
    m_animation = nullptr;
}

bool BVHAsset::IsLoaded()
{
    return m_animation != nullptr;
}

} // namespace Engine::Assets
