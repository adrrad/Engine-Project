#pragma once
#include "assets/ShaderAsset.hpp"

namespace Engine::Assets
{

ShaderAsset::ShaderAsset(Platform::IO::File* resourceFile) : Asset(resourceFile)
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
