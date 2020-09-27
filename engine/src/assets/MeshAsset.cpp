#pragma once
#include "assets/MeshAsset.hpp"

namespace Engine::Assets
{

MeshAsset::MeshAsset(Platform::IO::File* resourceFile, AssetID id) : Asset(resourceFile, id)
{

}

void MeshAsset::Load()
{

}

void MeshAsset::Free()
{

}

bool MeshAsset::IsLoaded()
{
    return m_meshData != nullptr;
}

} // namespace Engine::Assets
