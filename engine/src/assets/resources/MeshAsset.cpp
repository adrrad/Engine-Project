#pragma once
#include "assets/resources/MeshAsset.hpp"
#include "assets/importing/MeshImporter.hpp"

#include <imgui.h>

namespace Engine::Assets
{

MeshAsset::MeshAsset(Platform::IO::File* resourceFile, AssetID id) 
    : Asset(resourceFile, id)
{

}

void MeshAsset::Load()
{
    if(IsLoaded()) return;
    m_meshData = Importing::MeshImporter::ImportMesh(ResourceFile);
}

void MeshAsset::Free()
{
    if(IsLoaded())
    {
        delete m_meshData;
        m_meshData = nullptr;
    }
}

bool MeshAsset::IsLoaded()
{
    return m_meshData != nullptr;
}

void MeshAsset::EditorGUI()
{
    //TODO: Implement this
}

} // namespace Engine::Assets
