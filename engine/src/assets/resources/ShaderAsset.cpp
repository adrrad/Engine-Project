#pragma once
#include "assets/resources/ShaderAsset.hpp"

namespace Engine::Assets
{

ShaderAsset::ShaderAsset(Platform::IO::File* resourceFile, AssetID id) : Asset(resourceFile, id)
{

}

void ShaderAsset::Load()
{

}

void ShaderAsset::Free()
{

}

bool ShaderAsset::IsLoaded()
{
    return false;
}

} // namespace Engine::Assets
