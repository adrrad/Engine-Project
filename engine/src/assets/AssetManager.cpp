#include "assets/AssetManager.hpp"

#include "assets/ImageAsset.hpp"
#include "assets/MeshAsset.hpp"
#include "assets/ScriptAsset.hpp"
#include "assets/ShaderAsset.hpp"
#include "assets/JSONAsset.hpp"

#include "platform/GUID.hpp"

using namespace Engine::Platform::IO;

namespace Engine::Assets
{

Asset* AssetManager::MakeAssetFromFile(Platform::IO::File* file)
{
    std::string extension = file->Extension;
    AssetID id = Platform::GenerateGUID();
    if(extension == ".obj") return new MeshAsset(file, id);
    if(extension == ".png") return new ImageAsset(file, id);
    if(extension == ".json") return new JSONAsset(file, id);
    if(extension == ".hpp" || extension == ".cpp") return new ScriptAsset(file, id);
    if(extension == ".vert" || extension == ".frag") return new ShaderAsset(file, id);
    return nullptr;
}

void AssetManager::ScanAssets()
{
    for(File* file : m_projectFiles)
    {
        Asset* asset = MakeAssetFromFile(file);
        if(asset != nullptr) m_assets.push_back(asset);
    }
}

AssetManager::AssetManager(Platform::IO::Path projectRoot) : m_projectFiles(projectRoot)
{
    ScanAssets();
}

void AssetManager::SaveAssetDatabase()
{
    Path root = m_projectFiles.GetRoot();
    Path dbPath = root.ToString()+"/assetdb.meta";
    Platform::IO::File dbFile(dbPath);

    


}

};