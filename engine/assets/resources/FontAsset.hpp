#pragma once
#include "assets/Asset.hpp"

namespace Assets
{

class FontAsset : public Asset
{
private:
    
    std::string m_fontData = "";

public:

    FontAsset(Platform::IO::File* resourceFile, AssetID id);

    void Load() override;

    void Free() override;

    bool IsLoaded() override;

    std::string& GetFontData();
};

    
} // namespace Assets
