#pragma once
#include "assets/Asset.hpp"
#include "EngineData.hpp"

namespace Engine::Assets
{

class SkyboxAsset : public Asset
{
private:
    AssetID m_frontImage;
    AssetID m_backImage;
    AssetID m_topImage;
    AssetID m_botImage;
    AssetID m_leftImage;
    AssetID m_rightImage;
public:

    SkyboxAsset(Platform::IO::File* resourceFile, AssetID id);

    void Load() override;

    void Free() override;

    bool IsLoaded() override;

};

    
} // namespace Engine::Assets
