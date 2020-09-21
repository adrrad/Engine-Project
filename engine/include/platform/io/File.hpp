#pragma once

#include "utilities/Array.hpp"

#include <string>


namespace Engine::Platform::IO
{

typedef uint64_t FileSize;

enum class ResourceType { TEXT, MODEL_3D, IMAGE, JSON, OTHER };

class File
{
private:

    static ResourceType GetResourceTypeFromExtension(std::string extension);

public:
    Utilities::Array<char> Data;
    const FileSize Size;
    const std::string Path;
    const std::string Extension;
    const ResourceType Type;

    
    File(std::string absolutePath, char* data, FileSize size);
    
    ~File();

    void Write();

};


File* Open(std::string absolutePath);


} // namespace Engine::Platform
