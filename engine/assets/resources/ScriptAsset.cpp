#pragma once
#include "assets/resources/ScriptAsset.hpp"

namespace Assets
{

ScriptAsset::ScriptAsset(Platform::IO::File* resourceFile, AssetID id) : Asset(resourceFile, id)
{

}

void ScriptAsset::Load()
{

}

void ScriptAsset::Free()
{

}

bool ScriptAsset::IsLoaded()
{
    return false;
}

} // namespace Assets