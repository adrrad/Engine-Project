#include "assets/importing/ImageImporter.hpp"

// #define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace Assets::Importing
{
    
ImageData* ImageImporter::ImportImage(Platform::IO::File* imageFile)
{
    std::string path = imageFile->GetAbsolutePath().ToString();
    const char* imgPath = path.c_str();
    int width;
    int height;
    int channels;
    unsigned char* data = stbi_load(imgPath, &width, &height, &channels, 0);
    Array<unsigned char> imagedata = Array<unsigned char>(width*height*channels, data);
    return new ImageData(imagedata, width, height, channels);
}

} // namespace Assets::Importing
