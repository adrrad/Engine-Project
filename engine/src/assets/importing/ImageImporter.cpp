#include "assets/importing/ImageImporter.hpp"

// #define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace Engine::Assets::Importing
{
    
ImageData* ImageImporter::ImportImage(Platform::IO::File* meshFile)
{
    std::string path = meshFile->GetAbsolutePath().ToString();
    const char* imgPath = path.c_str();
    int width;
    int height;
    int channels;
    unsigned char* data = stbi_load(imgPath, &width, &height, &channels, 0);
    Array<unsigned char> imagedata = Array<unsigned char>(width*height*channels, data);
    return new ImageData(imagedata, width, height, channels);
}

} // namespace Engine::Assets::Importing
