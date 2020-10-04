#pragma once
#include "assets/Asset.hpp"
#include "assets/resources/ImageAsset.hpp"
#include "EngineData.hpp"

namespace Engine::Assets
{

struct CubemapImages
{
    ImageData* Right;
    ImageData* Left;
    ImageData* Top;
    ImageData* Bot;
    ImageData* Back;
    ImageData* Front;
    CubemapImages(ImageData* right,
                ImageData* left,
                ImageData* top,
                ImageData* bot,
                ImageData* back,
                ImageData* front);
};

class CubemapAsset : public Asset
{
private:
    AssetID m_frontImage;
    AssetID m_backImage;
    AssetID m_topImage;
    AssetID m_botImage;
    AssetID m_leftImage;
    AssetID m_rightImage;
public:
    enum CubemapSide { RIGHT, LEFT, TOP, BOT, BACK, FRONT};

    CubemapAsset(Platform::IO::File* resourceFile, AssetID id);

    void Load() override;

    void Free() override;

    bool IsLoaded() override;

    ImageAsset* GetSide(CubemapSide side);

    CubemapImages GetImages();

};

    
} // namespace Engine::Assets
