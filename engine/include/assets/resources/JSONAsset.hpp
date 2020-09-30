#pragma once
#include "assets/Asset.hpp"

namespace Engine::Assets
{

class JSONAsset : public Asset
{
private:
    
public:

    JSONAsset(Platform::IO::File* resourceFile, AssetID id);

    void Load() override;

    void Free() override;

    bool IsLoaded() override;

};

    
} // namespace Engine::Assets
