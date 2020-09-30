#pragma once
#include "assets/resources/SkyboxAsset.hpp"
#include "assets/resources/ImageAsset.hpp"
#include "assets/AssetManager.hpp"

namespace Engine::Assets
{

SkyboxAsset::SkyboxAsset(Platform::IO::File* resourceFile, AssetID id) : Asset(resourceFile, id)
{

}

void SkyboxAsset::Load()
{
    if(IsLoaded()) return;
    AssetManager& mng = *AssetManager::GetInstance();
    mng.GetAsset<ImageAsset>(m_frontImage)->Load();
    mng.GetAsset<ImageAsset>(m_backImage)->Load();
    mng.GetAsset<ImageAsset>(m_topImage)->Load();
    mng.GetAsset<ImageAsset>(m_botImage)->Load();
    mng.GetAsset<ImageAsset>(m_leftImage)->Load();
    mng.GetAsset<ImageAsset>(m_rightImage)->Load();
}

void SkyboxAsset::Free()
{

}

bool SkyboxAsset::IsLoaded()
{
    AssetManager& mng = *AssetManager::GetInstance();
    return mng.IsLoaded(m_frontImage)
        && mng.IsLoaded(m_backImage)
        && mng.IsLoaded(m_topImage)
        && mng.IsLoaded(m_botImage)
        && mng.IsLoaded(m_leftImage)
        && mng.IsLoaded(m_rightImage);
}

} // namespace Engine::Assets
