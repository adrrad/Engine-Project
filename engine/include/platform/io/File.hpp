#pragma once

#include "platform/io/Path.hpp"
#include "Array.hpp"
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
    const std::string FileName;
    const std::string Extension;

    File(Path absolutePath);
    
    ~File();

    void Write(char* data);

    Array<char> ReadAll();
};



} // namespace Engine::Platform
