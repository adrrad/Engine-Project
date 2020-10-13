#pragma once

#include "platform/io/Path.hpp"
#include "Array.hpp"

#include <string>
#include <fstream>

namespace Engine::Platform::IO
{

typedef uint64_t FileSize;




class File
{
private:
    static FileSize GetSize(Path path);
    std::fstream stream;
public:

    enum OpenMode
    {
        READ = 1,
        WRITE = 2,
        ATEND = 4,
        APPEND = 8,
        TRUNCATE = 16,
        BINARY = 32,
    };
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

    void Write(const char* data);

    void Write(std::string data);

    Array<char> ReadAll();
};


inline File::OpenMode operator|(File::OpenMode a, File::OpenMode b)
{
    return static_cast<File::OpenMode>(static_cast<int>(a) | static_cast<int>(b));
}

inline File::OpenMode operator&(File::OpenMode a, File::OpenMode b)
{
    return static_cast<File::OpenMode>(static_cast<int>(a) & static_cast<int>(b));
}

} // namespace Engine::Platform
