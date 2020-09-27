#pragma once
#include "assets/ScriptAsset.hpp"

namespace Engine::Assets
{

ScriptAsset::ScriptAsset(Platform::IO::File* resourceFile) : Asset(resourceFile)
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
