#include "assets/AssetManager.hpp"

#include "assets/resources/ImageAsset.hpp"
#include "assets/resources/MeshAsset.hpp"
#include "assets/resources/ScriptAsset.hpp"
#include "assets/resources/ShaderAsset.hpp"
#include "assets/resources/JSONAsset.hpp"

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
    dbFile.Open(Platform::IO::File::TRUNCATE | Platform::IO::File::WRITE);
    for(auto asset : m_assets)
    {
        std::string guid = asset->ID.ToString();
        std::string path = m_projectFiles.GetRelativePath(asset->ResourceFile->FilePath).ToString();
        std::string info = guid + " : " + path + "\n";
        dbFile.Write(info.c_str());
    }
    dbFile.Close();
}

};