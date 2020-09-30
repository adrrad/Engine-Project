#include "assets/importing/ImageImporter.hpp"

// #define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace Engine::Assets::Importing
{
    
ImageData* ImageImporter::ImportImage(Platform::IO::File* meshFile)
{
    std::string path = meshFile->FilePath.ToString();
    const char* imgPath = path.c_str();
    int width;
    int height;
    int channels;
    unsigned char* data = stbi_load(imgPath, &width, &height, &channels, 0);
    return new ImageData(Array<unsigned char>(width*height*channels, data), width, height, channels);
}

} // namespace Engine::Assets::Importing
