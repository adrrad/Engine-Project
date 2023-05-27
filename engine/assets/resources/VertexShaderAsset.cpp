#pragma once
#include "assets/resources/VertexShaderAsset.hpp"

namespace Assets
{

VertexShaderAsset::VertexShaderAsset(Platform::IO::File* resourceFile, AssetID id) : Asset(resourceFile, id)
{

}

void VertexShaderAsset::Load()
{

}

void VertexShaderAsset::Free()
{

}

bool VertexShaderAsset::IsLoaded()
{
    return false;
}

} // namespace Assets
