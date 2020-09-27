#pragma once

#include "assets/Asset.hpp"

#include "platform/io/File.hpp"
#include "platform/io/Filesystem.hpp"
#include "utilities/serialisation/Serialisation.hpp"


namespace Engine::Assets
{

class AssetManager final
{
private:
    Platform::IO::Filesystem m_projectFiles;
    std::vector<Asset*> m_assets;

    Asset* MakeAssetFromFile(Platform::IO::File* file);

    void ScanAssets();

public:
    AssetManager(Platform::IO::Path projectRoot);

};

    
} // namespace Engine::Assets
