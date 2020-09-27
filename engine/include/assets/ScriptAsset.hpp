#pragma once
#include "assets/Asset.hpp"

namespace Engine::Assets
{

class ScriptAsset : public Asset
{
private:
    
public:

    ScriptAsset(Platform::IO::File* resourceFile, AssetID id);

    void Load() override;

    void Free() override;

    bool IsLoaded() override;

};

    
} // namespace Engine::Assets
