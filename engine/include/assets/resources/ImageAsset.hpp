#pragma once
#include "assets/Asset.hpp"
#include "EngineData.hpp"

namespace Engine::Assets
{

class ImageAsset : public Asset
{
private:
    ImageData* m_imageData = nullptr;
public:

    ImageAsset(Platform::IO::File* resourceFile, AssetID id);

    void Load() override;

    void Free() override;

    bool IsLoaded() override;

};

    
} // namespace Engine::Assets
