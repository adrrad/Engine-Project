#pragma once

#include "utilities/Array.hpp"
#include "platform/io/Path.hpp"

#include <string>


namespace Engine::Platform::IO
{

typedef uint64_t FileSize;


class File
{
private:
    static FileSize GetSize(Path path);
public:
    const FileSize Size;
    const Path FilePath;
    const std::string Extension;

    File(std::string absolutePath);
    
    ~File();

    void Write(char* data);

    char* ReadAll();
};



} // namespace Engine::Platform
