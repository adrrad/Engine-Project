#pragma once

#include "EngineData.hpp"
#include "platform/io/File.hpp"

namespace Assets::Importing
{
    
class ImageImporter
{
public:

static ImageData* ImportImage(Platform::IO::File* imageFile);

};

} // namespace Assets::Importing
