#pragma once

#include "platform/io/Path.hpp"
#include "Array.hpp"

#include <string>
#include <fstream>

namespace Engine::Platform::IO
{
typedef uint64_t FileSize;

class Filesystem;



class File
{
friend class Filesystem;
private:
    static FileSize GetSize(Path path);

    Filesystem* m_parentFilesystem = nullptr;
    std::fstream m_stream;
    std::string m_name;
    std::string m_extension;
    FileSize m_size;
    Path m_path;

    File(Filesystem* fs, Path relativePath);
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


    File(Path absolutePath);

    File(const File& other);

    File& operator=(const File& other);
    
    ~File();

    bool IsOpen();

    void Open(OpenMode mode);

    void Close();

    void Write(const char* data);

    void Write(std::string data);

    inline const std::string& GetName() const;

    inline const std::string& GetExtension() const;

    inline Path GetPath() const;

    Path GetAbsolutePath() const;

    inline FileSize GetSizeBytes() const;

    Array<char> ReadAll();

};

const std::string& File::GetName() const
{
    return m_name;
}

const std::string& File::GetExtension() const
{
    return m_extension;
}

inline Path File::GetPath() const
{
    return m_path;
}

FileSize File::GetSizeBytes() const
{
    return m_size;
}

inline File::OpenMode operator|(File::OpenMode a, File::OpenMode b)
{
    return static_cast<File::OpenMode>(static_cast<int>(a) | static_cast<int>(b));
}

inline File::OpenMode operator&(File::OpenMode a, File::OpenMode b)
{
    return static_cast<File::OpenMode>(static_cast<int>(a) & static_cast<int>(b));
}

} // namespace Engine::Platform
