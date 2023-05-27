#include "assets/AssetManager.hpp"

#include "core/EngineSettings.hpp"
#include "memory/MemoryManager.hpp"
#include "memory/Allocator.hpp"
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
#include "utilities/Utilities.hpp"
#include "utilities/json/JSON.hpp"

#include <future>
#include <execution>

using namespace Platform::IO;
using namespace Utilities;
using namespace Core;
using namespace Memory;

namespace Assets
{

void AssetManager::Initialise() 
{
    u64 memory = EngineSettings::Get().Memory.AssetsMemory;
    SetAllocator(MemoryManager::GetInstance()->CreateAllocator<AssetManager>(memory));

    LoadAssetDatabase();
    ScanAssets();
    SaveAssetDatabase();
}

Asset* AssetManager::MakeAssetFromFile(Platform::IO::File* file, AssetID id)
{
    if(!id.IsAssigned()) id = Platform::GenerateGUID();
    std::string extension = Utilities::ToLower(file->GetExtension());
    switch(CalcHash(extension.c_str()))
    {
        case CalcHash(".bvh"):
            return New<BVHAsset>(file, id);
        case CalcHash(".obj"):
            return New<MeshAsset>(file, id);
        case CalcHash(".json"):
            return New<JSONAsset>(file, id);
        case CalcHash(".ttf"):
            return New<FontAsset>(file, id);
        case CalcHash(".frag"):
            return New<FragmentShaderAsset>(file, id);
        case CalcHash(".vert"):
            return New<VertexShaderAsset>(file, id);
        case CalcHash(".cubemap"):
            return New<CubemapAsset>(file, id);
        case CalcHash(".hpp"):
        case CalcHash(".cpp"):
            return New<ScriptAsset>(file, id);
        case CalcHash(".png"):
        case CalcHash(".jpg"):
        case CalcHash(".jpeg"):
        case CalcHash(".tga"):
            return New<ImageAsset>(file, id);
        default:
            return nullptr;
    }
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
            asset->Load();
            m_assets.push_back(asset);
            m_assetTable.insert({asset->ID.ToString(), asset});
            m_assetTable.insert({file->GetPath().ToString(), asset});
        }
    };
    m_projectFiles.ForEachFile(manageAsset);
}

AssetManager::AssetManager() 
: m_projectFiles(Core::EngineSettings::Get().Project.RootDirectory)
{   
}

void AssetManager::LoadAssetDatabase()
{
    using namespace Utilities::JSON;
    Path dbPath = m_projectFiles.GetRootPath().ToString()+"assetdb.db";
    if(m_projectFiles.FileExists(dbPath))
    {
        File db(dbPath);
        db.Open(File::READ);
        Array<char> json = db.ReadAll();
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