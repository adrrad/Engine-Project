#pragma once
#include "assets/ScriptAsset.hpp"

namespace Engine::Assets
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

} // namespace Engine::Assets
