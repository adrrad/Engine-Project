#include "platform/io/File.hpp"

#include "Exceptions.hpp"
#include "utilities/StringUtilities.hpp"


#include <iostream>
#include <fstream>
namespace Engine::Platform::IO
{

FileSize File::GetSize(Path path)
{
    if(!std::filesystem::exists(std::filesystem::path(path.ToString()))) return 0;
    return std::filesystem::file_size(std::filesystem::path(path.ToString()));

}
    
File::File(Path absolutePath) :
    FilePath(Path(absolutePath)),
    FileName(absolutePath.GetFilename()),
    Extension(absolutePath.GetExtension()),
    Size(GetSize(Path(absolutePath)))
{

}

File::File(const File& other) :
    FilePath(other.FilePath),
    FileName(other.FileName),
    Extension(other.Extension),
    Size(other.Size)
{

}

File::~File()
{

}

bool File::IsOpen()
{
    return stream.is_open();
}

void File::Open(OpenMode mode)
{
    if(stream.is_open()) return;
    stream.open(FilePath.ToString(), std::ios::openmode(mode));
    if(stream.fail()) throw EngineException("Could not open file: " + FilePath.ToString());
}

void File::Close()
{
    if(!stream.is_open()) return;
    stream.close();
}

void File::Write(const char* data)
{
    if(!stream.is_open()) throw EngineException("File write error: File not open!");
    stream << data;
}

void File::Write(std::string data)
{
    if(!stream.is_open()) throw EngineException("File write error: File not open!");
    stream.write(data.c_str(), data.length());
}

Array<char> File::ReadAll()
{
    std::ifstream file(FilePath.ToString(), std::ios::in | std::ios::binary);
    std::string str(std::istreambuf_iterator<char>{file}, {});

    char* data = new char[str.size()];
    memcpy(data, str.c_str(), str.size());
    return Array<char>(str.size(), data);
}

File& File::operator=(const File& other)
{
    Size = other.Size;
    FilePath = other.FilePath;
    FileName = other.FileName;
    Extension = other.Extension;
    return *this;
}

} // namespace Engine::Platform::IO

