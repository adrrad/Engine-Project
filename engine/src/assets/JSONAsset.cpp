#pragma once
#include "assets/JSONAsset.hpp"

namespace Engine::Assets
{

JSONAsset::JSONAsset(Platform::IO::File* resourceFile, AssetID id) : Asset(resourceFile, id)
{

}

void JSONAsset::Load()
{

}

void JSONAsset::Free()
{

}

bool JSONAsset::IsLoaded()
{
    return false;
}

} // namespace Engine::Assets
