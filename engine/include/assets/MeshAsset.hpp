#pragma once
#include "assets/Asset.hpp"
#include "assets/importing/MeshImporter.hpp"
namespace Engine::Assets
{

class MeshAsset : public Asset
{
private:

    char* m_meshData;

public:

    MeshAsset(Platform::IO::File* resourceFile, AssetID id);

    void Load() override;

    void Free() override;

    bool IsLoaded() override;

};


} // namespace Engine::Assets
