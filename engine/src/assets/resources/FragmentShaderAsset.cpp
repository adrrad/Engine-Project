#pragma once
#include "assets/resources/FragmentShaderAsset.hpp"

namespace Engine::Assets
{

FragmentShaderAsset::FragmentShaderAsset(Platform::IO::File* resourceFile, AssetID id) : Asset(resourceFile, id)
{

}

void FragmentShaderAsset::Load()
{

}

void FragmentShaderAsset::Free()
{

}

bool FragmentShaderAsset::IsLoaded()
{
    return false;
}

} // namespace Engine::Assets
