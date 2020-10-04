#pragma once
#include "assets/resources/CubemapAsset.hpp"
#include "assets/resources/ImageAsset.hpp"
#include "assets/AssetManager.hpp"

namespace Engine::Assets
{

CubemapImages::CubemapImages(ImageData* right,
                ImageData* left,
                ImageData* top,
                ImageData* bot,
                ImageData* back,
                ImageData* front) 
{
    Right = right;
    Left = left; 
    Top = top; 
    Bot = bot;
    Back = back;
    Front = front;
}

CubemapAsset::CubemapAsset(Platform::IO::File* resourceFile, AssetID id) : Asset(resourceFile, id)
{

}

void CubemapAsset::Load()
{
    if(IsLoaded()) return;
    AssetManager* mng = AssetManager::GetInstance();
    mng->GetAsset<ImageAsset>(m_frontImage)->Load();
    mng->GetAsset<ImageAsset>(m_backImage)->Load();
    mng->GetAsset<ImageAsset>(m_topImage)->Load();
    mng->GetAsset<ImageAsset>(m_botImage)->Load();
    mng->GetAsset<ImageAsset>(m_leftImage)->Load();
    mng->GetAsset<ImageAsset>(m_rightImage)->Load();
}

void CubemapAsset::Free()
{

}

bool CubemapAsset::IsLoaded()
{
    AssetManager* mng = AssetManager::GetInstance();
    return mng->IsLoaded(m_frontImage)
        && mng->IsLoaded(m_backImage)
        && mng->IsLoaded(m_topImage)
        && mng->IsLoaded(m_botImage)
        && mng->IsLoaded(m_leftImage)
        && mng->IsLoaded(m_rightImage);
}

ImageAsset* CubemapAsset::GetSide(CubemapSide side)
{
    AssetManager* mng = AssetManager::GetInstance();
    switch (side)
    {
    case RIGHT:
        return mng->GetAsset<ImageAsset>(m_rightImage);
    case LEFT:
        return mng->GetAsset<ImageAsset>(m_leftImage);
    case TOP:
        return mng->GetAsset<ImageAsset>(m_topImage);
    case BOT:
        return mng->GetAsset<ImageAsset>(m_botImage);
    case BACK:
        return mng->GetAsset<ImageAsset>(m_backImage);
    case FRONT:
        return mng->GetAsset<ImageAsset>(m_frontImage);
    default:
        throw EngineException("Unknown Cubemap side: " + side);
    }
}

CubemapImages CubemapAsset::GetImages()
{
    return CubemapImages(
        GetSide(RIGHT)->GetImageData(),
        GetSide(LEFT)->GetImageData(),
        GetSide(TOP)->GetImageData(),
        GetSide(BOT)->GetImageData(),
        GetSide(BACK)->GetImageData(),
        GetSide(FRONT)->GetImageData()
    );
}

} // namespace Engine::Assets
