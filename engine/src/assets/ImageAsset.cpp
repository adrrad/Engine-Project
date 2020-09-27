#pragma once
#include "assets/ImageAsset.hpp"

namespace Engine::Assets
{

ImageAsset::ImageAsset(Platform::IO::File* resourceFile, AssetID id) : Asset(resourceFile, id)
{

}

void ImageAsset::Load()
{

}

void ImageAsset::Free()
{

}

bool ImageAsset::IsLoaded()
{
    return false;
}

} // namespace Engine::Assets
