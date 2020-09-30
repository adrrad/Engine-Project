#include "assets/AssetManager.hpp"

#include "assets/resources/ImageAsset.hpp"
#include "assets/resources/MeshAsset.hpp"
#include "assets/resources/ScriptAsset.hpp"
#include "assets/resources/ShaderAsset.hpp"
#include "assets/resources/FragmentShaderAsset.hpp"
#include "assets/resources/VertexShaderAsset.hpp"
#include "assets/resources/JSONAsset.hpp"

#include "platform/GUID.hpp"

#include "Exceptions.hpp"

using namespace Engine::Platform::IO;

namespace Engine::Assets
{

AssetManager* AssetManager::m_instance = nullptr;

Asset* AssetManager::MakeAssetFromFile(Platform::IO::File* file)
{
    std::string extension = file->Extension;
    AssetID id = Platform::GenerateGUID();
    if(extension == ".obj") return new MeshAsset(file, id);
    if(extension == ".png") return new ImageAsset(file, id);
    if(extension == ".json") return new JSONAsset(file, id);
    if(extension == ".hpp" || extension == ".cpp") return new ScriptAsset(file, id);
    if(extension == ".shader") return new ShaderAsset(file, id);
    if(extension == ".frag") return new FragmentShaderAsset(file, id);
    if(extension == ".vert") return new VertexShaderAsset(file, id);
    return nullptr;
}

void AssetManager::ScanAssets()
{
    for(File* file : m_projectFiles)
    {
        Asset* asset = MakeAssetFromFile(file);
        if(asset != nullptr)
        {
            m_assets.push_back(asset);
            m_assetTable.insert({asset->ID.value, asset});
        }
    }
}

AssetManager::AssetManager(Platform::IO::Path projectRoot) : m_projectFiles(projectRoot)
{
    ScanAssets();
    if(m_instance != nullptr) throw EngineException("Engine error: trying to instantiate the AssetManager");
    m_instance = this;
}

Asset* AssetManager::GetAsset(Platform::IO::Path relativepath)
{
    throw EngineException("Not implemented!");
}

Asset* AssetManager::GetAsset(const AssetID& id)
{
    std::string guid = id.value;
    if(!m_assetTable.contains(guid)) throw EngineException("Asset with GUID: " + guid + "' does not exist!");
    return m_assetTable[guid];
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