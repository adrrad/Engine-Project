#pragma once
#include "assets/Asset.hpp"
#include "EngineData.hpp"

namespace Engine::Assets
{

class MeshAsset : public Asset
{
private:

    MeshData* m_meshData = nullptr;

public:

    MeshAsset(Platform::IO::File* resourceFile, AssetID id);

    void Load() override;

    void Free() override;

    bool IsLoaded() override;

    void EditorGUI() override;

    inline MeshData* GetMeshData();

};

MeshData* MeshAsset::GetMeshData()
{
    if(!IsLoaded()) Load();
    return m_meshData;
}

} // namespace Engine::Assets
