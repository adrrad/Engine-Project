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
    
File::File(Path absolutePath)
{
    m_path = absolutePath;
    m_name = absolutePath.GetFilename();
    m_extension = absolutePath.GetExtension();
    m_size = GetSize(absolutePath);
}

File::File(const File& other)
{
    m_path = other.GetPath();
    m_name = other.GetName();
    m_extension = other.GetExtension();
    m_size = other.GetSizeBytes();
}

File::~File()
{

}

bool File::IsOpen()
{
    return m_stream.is_open();
}

void File::Open(OpenMode mode)
{
    if(m_stream.is_open()) return;
    m_stream.open(m_path.ToString(), std::ios::openmode(mode));
    if(m_stream.fail()) throw EngineException("Could not open file: " + m_path.ToString());
}

void File::Close()
{
    if(!m_stream.is_open()) return;
    m_stream.close();
}

void File::Write(const char* data)
{
    if(!m_stream.is_open()) throw EngineException("File write error: File not open!");
    m_stream << data;
}

void File::Write(std::string data)
{
    if(!m_stream.is_open()) throw EngineException("File write error: File not open!");
    m_stream.write(data.c_str(), data.length());
}

Array<char> File::ReadAll()
{
    std::ifstream file(m_path.ToString(), std::ios::in | std::ios::binary);
    std::string str(std::istreambuf_iterator<char>{file}, {});
    char* data = new char[str.size()];
    memcpy(data, str.c_str(), str.size());
    return Array<char>(str.size(), data);
}

File& File::operator=(const File& other)
{
    m_size = other.GetSizeBytes();
    m_path = other.GetPath();
    m_name = other.GetName();
    m_extension = other.GetExtension();
    return *this;
}

} // namespace Engine::Platform::IO

