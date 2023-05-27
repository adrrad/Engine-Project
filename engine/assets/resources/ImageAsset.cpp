#include "assets/resources/ImageAsset.hpp"
#include "assets/importing/ImageImporter.hpp"
namespace Assets
{

ImageAsset::ImageAsset(Platform::IO::File* resourceFile, AssetID id) : Asset(resourceFile, id)
{

}

void ImageAsset::Load()
{
    if(IsLoaded()) return;
    m_imageData = Importing::ImageImporter::ImportImage(ResourceFile);
}

void ImageAsset::Free()
{
    if(IsLoaded())
    {
        delete m_imageData;
        m_imageData = nullptr;
    }
}

bool ImageAsset::IsLoaded()
{
    return m_imageData != nullptr;
}

} // namespace Assets
