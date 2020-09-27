#pragma once
#include "assets/Asset.hpp"

namespace Engine::Assets
{

class ImageAsset : public Asset
{
private:
    
public:

    ImageAsset(Platform::IO::File* resourceFile);

    void Load() override;

    void Free() override;

    bool IsLoaded() override;

};

    
} // namespace Engine::Assets
