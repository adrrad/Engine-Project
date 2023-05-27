#pragma once

#include "EngineComponent.hpp"
#include "assets/Asset.hpp"

#include "platform/io/File.hpp"
#include "platform/io/Filesystem.hpp"
#include "platform/GUID.hpp"

#include "Debug.hpp"

#include "utilities/StringUtilities.hpp"
#include <unordered_map>

namespace Core
{
    class EngineCore;
}

namespace Assets
{

class AssetManager : public EngineSubsystem<AssetManager>
{
friend class EngineSubsystem<AssetManager>;
private:

    Platform::IO::Filesystem m_projectFiles;

    std::vector<Asset*> m_assets;

    std::unordered_map<std::string, Asset*> m_assetTable;
    
    Asset* MakeAssetFromFile(Platform::IO::File* file, AssetID id = AssetID());

    void ScanAssets();

    AssetManager();

    void LoadAssetDatabase();

public:

    void Initialise() override;

    template <class T>
    T* CreateAsset(Platform::IO::Path relativePath);

    template <class T>
    T* GetAsset(Platform::IO::Path relativePath);

    template <class T>
    inline T* GetAsset(const AssetID& id);

    inline bool IsLoaded(const AssetID& id);

    void SaveAssetDatabase();

    inline Platform::IO::Filesystem* GetFilesystem();
};

template <class T>
T* AssetManager::CreateAsset(Platform::IO::Path relativePath)
{
    Platform::IO::File* resourceFile = m_projectFiles.CreateFile(relativePath.ToString());
    T* asset = new T(resourceFile, Platform::GenerateGUID());
    m_assets.push_back(asset);
    m_assetTable.insert({asset->ID.ToString(), asset});
    m_assetTable.insert({relativePath.ToString(), asset});
    ScanAssets();
    return asset;
}


template <class T>
T* AssetManager::GetAsset(Platform::IO::Path relativePath)
{
    std::string path = relativePath.ToString();// Utilities::Replace(relativePath.ToString(), "/", "\\");
    if(!m_assetTable.contains(path)) return nullptr;
    T* asset = dynamic_cast<T*>(m_assetTable[path]);
    if(!asset)
    {
        std::string typeName = typeid(T).name();
        AssertFail("Asset with path: " + path + " is not an instance of type '" + typeName +"'!");
    }
    return asset;
}

template <class T>
T* AssetManager::GetAsset(const AssetID& id)
{
    std::string guid = id.ToString();
    if(!m_assetTable.contains(guid)) AssertFail("Asset with GUID '" + guid + "' does not exist!");
    T* asset = dynamic_cast<T*>(m_assetTable[guid]);
    if(!asset)
    {
        std::string typeName = typeid(T).name();
        AssertFail("Asset with GUID: " + id.ToString() + " is not an instance of type '" + typeName +"'!");
    }
    return asset;
}

bool AssetManager::IsLoaded(const AssetID& id)
{
    return m_assetTable[id.ToString()]->IsLoaded();
}

Platform::IO::Filesystem* AssetManager::GetFilesystem()
{
    return &m_projectFiles;
}
    
} // namespace Assets
