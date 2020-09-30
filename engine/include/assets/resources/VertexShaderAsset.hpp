#pragma once
#include "assets/Asset.hpp"

namespace Engine::Assets
{

class VertexShaderAsset : public Asset
{
private:
    
public:
    VertexShaderAsset(Platform::IO::File* resourceFile, AssetID id);

    void Load() override;

    void Free() override;

    bool IsLoaded() override;

};

    
} // namespace Engine::Assets
