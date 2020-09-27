#include "assets/AssetManager.hpp"

#include "assets/ImageAsset.hpp"
#include "assets/MeshAsset.hpp"
#include "assets/ScriptAsset.hpp"
#include "assets/ShaderAsset.hpp"
#include "assets/JSONAsset.hpp"

using namespace Engine::Platform::IO;

namespace Engine::Assets
{

Asset* AssetManager::MakeAssetFromFile(Platform::IO::File* file)
{
    std::string extension = file->Extension;

    if(extension == ".obj") return new MeshAsset(file);
    if(extension == ".png") return new ImageAsset(file);
    if(extension == ".json") return new JSONAsset(file);
    if(extension == ".hpp" || extension == ".cpp") return new ScriptAsset(file);
    if(extension == ".vert" || extension == ".frag") return new ShaderAsset(file);
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

};