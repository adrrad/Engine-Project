#pragma once

#include "assets/Asset.hpp"

#include "platform/io/File.hpp"
#include "platform/io/Filesystem.hpp"
#include "Exceptions.hpp"

#include <unordered_map>

namespace Engine::Core
{
    class EngineCore;
}

namespace Engine::Assets
{

class AssetManager final
{
friend class Core::EngineCore;

private:
    static AssetManager* m_instance;

    Platform::IO::Filesystem m_projectFiles;

    std::vector<Asset*> m_assets;

    std::unordered_map<std::string, Asset*> m_assetTable;
    
    Asset* MakeAssetFromFile(Platform::IO::File* file);

    void ScanAssets();

    AssetManager(Platform::IO::Path projectRoot);
public:
    static inline AssetManager* GetInstance() 
    { 
        if(m_instance == nullptr) throw EngineException("Fatal error: No AssetManager instance exists.");
        return m_instance; 
    }

    Asset* GetAsset(Platform::IO::Path relativepath);

    Asset* GetAsset(const AssetID& id);

    void SaveAssetDatabase();
};

    
} // namespace Engine::Assets
