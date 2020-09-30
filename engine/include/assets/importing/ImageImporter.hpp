#pragma once

#include "EngineData.hpp"
#include "platform/io/File.hpp"

namespace Engine::Assets::Importing
{
    
class ImageImporter
{
public:

static ImageData* ImportImage(Platform::IO::File* meshFile);

};

} // namespace Engine::Assets::Importing
