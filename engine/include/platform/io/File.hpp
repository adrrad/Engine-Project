#pragma once

#include "platform/io/Path.hpp"
#include "Array.hpp"

#include <string>
#include <fstream>

namespace Engine::Platform::IO
{

typedef uint64_t FileSize;

enum class OpenMode
{
    READ = 1,
    WRITE = 2,
    ATEND = 4,
    APPEND = 8,
    TRUNCATE = 16,
    BINARY = 32,
};

inline OpenMode operator|(OpenMode a, OpenMode b)
{
    return static_cast<OpenMode>(static_cast<int>(a) | static_cast<int>(b));
}

inline OpenMode operator&(OpenMode a, OpenMode b)
{
    return static_cast<OpenMode>(static_cast<int>(a) & static_cast<int>(b));
}

class File
{
private:
    static FileSize GetSize(Path path);
    std::fstream stream;
public:
    const FileSize Size;
    const Path FilePath;
    const std::string FileName;
    const std::string Extension;

    File(Path absolutePath);

    File(const File& other);
    
    ~File();

    bool IsOpen();

    void Open(OpenMode mode);

    void Close();

    void Write(char* data);

    Array<char> ReadAll();
};



} // namespace Engine::Platform
