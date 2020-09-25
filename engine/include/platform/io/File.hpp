#pragma once

#include "utilities/Array.hpp"

#include <string>


namespace Engine::Platform::IO
{

typedef uint64_t FileSize;


class File
{
private:

public:
    const FileSize Size;
    const std::string Path;
    const std::string Extension;

    File(std::string absolutePath, char* data, FileSize size);
    
    ~File();

    void Write(char* data);

};


File* Open(std::string absolutePath);


} // namespace Engine::Platform
