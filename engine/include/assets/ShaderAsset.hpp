#pragma once
#include "assets/Asset.hpp"

namespace Engine::Assets
{

class ShaderAsset : public Asset
{
private:
    
public:
    ShaderAsset(Platform::IO::File* resourceFile);

    void Load() override;

    void Free() override;

    bool IsLoaded() override;

};

    
} // namespace Engine::Assets
