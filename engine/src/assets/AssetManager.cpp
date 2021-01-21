#include "assets/AssetManager.hpp"

#include "assets/resources/ImageAsset.hpp"
#include "assets/resources/CubemapAsset.hpp"
#include "assets/resources/MeshAsset.hpp"
#include "assets/resources/ScriptAsset.hpp"
#include "assets/resources/ShaderAsset.hpp"
#include "assets/resources/FragmentShaderAsset.hpp"
#include "assets/resources/VertexShaderAsset.hpp"
#include "assets/resources/JSONAsset.hpp"
#include "assets/resources/FontAsset.hpp"
#include "assets/resources/BVHAsset.hpp"

#include "platform/GUID.hpp"

#include "utilities/StringUtilities.hpp"
#include "utilities/json/JSON.hpp"

#include "Exceptions.hpp"

using namespace Engine::Platform::IO;

namespace Engine::Assets
{

AssetManager* AssetManager::m_instance = nullptr;

Asset* AssetManager::MakeAssetFromFile(Platform::IO::File* file, AssetID id)
{
    std::string extension = Utilities::ToLower(file->GetExtension());
    if(!id.IsAssigned()) id = Platform::GenerateGUID();
    if(extension == ".bvh") return new BVHAsset(file, id);
    if(extension == ".obj") return new MeshAsset(file, id);
    if(extension == ".json") return new JSONAsset(file, id);
    if(extension == ".ttf") return new FontAsset(file, id);
    if(extension == ".shader") return new ShaderAsset(file, id);
    if(extension == ".frag") return new FragmentShaderAsset(file, id);
    if(extension == ".vert") return new VertexShaderAsset(file, id);
    if(extension == ".cubemap") return new CubemapAsset(file, id);
    if(extension == ".hpp" || extension == ".cpp") return new ScriptAsset(file, id);
    if(Utilities::Match(extension, { ".png", ".jpg", ".jpeg", ".tga" })) return new ImageAsset(file, id);
    return nullptr;
}

void AssetManager::ScanAssets()
{
    auto manageAsset = [&](Path filePath)
    {
        File* file = m_projectFiles.GetFile(filePath);
        if(m_assetTable.contains(file->GetPath().ToString())) return;
        Asset* asset = MakeAssetFromFile(file);
        if(asset != nullptr)
        {
            m_assets.push_back(asset);
            m_assetTable.insert({asset->ID.ToString(), asset});
            m_assetTable.insert({file->GetPath().ToString(), asset});
        }
    };
    m_projectFiles.ForEachFile(manageAsset);
}

AssetManager::AssetManager(Platform::IO::Path projectRoot) : m_projectFiles(projectRoot)
{
    if(m_instance != nullptr) throw EngineException("Engine error: trying to instantiate the AssetManager");
    m_instance = this;
    LoadAssetDatabase();
    ScanAssets();
    SaveAssetDatabase();
}

void AssetManager::LoadAssetDatabase()
{
    using namespace Utilities::JSON;
    Path dbPath = m_projectFiles.GetRootPath().ToString()+"assetdb.db";
    if(m_projectFiles.FileExists(dbPath))
    {
        File db(dbPath);
        db.Open(File::READ);
        Engine::Array<char> json = db.ReadAll();
        db.Close();
        if(json.Length == 0) return;
        std::string jsonString = std::string(json.Data(), json.Length);
        auto obj = ParseJSON(jsonString);
        auto mapping = obj->Members;
        for(auto& kv : mapping)
        {
            auto filePath = kv.Key;
            auto guid = kv.Value->String;
            File* resourceFile = m_projectFiles.GetFile(filePath);
            Asset* asset = MakeAssetFromFile(resourceFile, AssetID(guid));
            m_assets.push_back(asset);
            m_assetTable.insert({asset->ID.ToString(), asset});
            m_assetTable.insert({resourceFile->GetPath().ToString(), asset});
        }
    }
}

void AssetManager::SaveAssetDatabase()
{
    using namespace Utilities::JSON;
    Path root = m_projectFiles.GetRootPath();
    Path dbPath = root.ToString()+"assetdb.db";
    std::vector<JSONKeyValuePair> values;
    for(auto asset : m_assets)
    {
        std::string guid = asset->ID.ToString();
        std::shared_ptr<JSONValue> val = JSONValue::AsString(guid);
        std::string path = asset->ResourceFile->GetPath().ToString();
        values.push_back(JSONKeyValuePair(path, val));
    }
    auto obj = JSONValue::AsObject(values);
    auto json = obj->ToString();
    Platform::IO::File dbFile(dbPath);
    dbFile.Open(Platform::IO::File::TRUNCATE | Platform::IO::File::WRITE);
    dbFile.Write(json);
    dbFile.Close();
}

};