#pragma once
#include "assets/Asset.hpp"

namespace Engine::Assets
{

class FragmentShaderAsset : public Asset
{
private:
    
public:
    FragmentShaderAsset(Platform::IO::File* resourceFile, AssetID id);

    void Load() override;

    void Free() override;

    bool IsLoaded() override;

};

    
} // namespace Engine::Assets
