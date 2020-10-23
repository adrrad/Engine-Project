#include "platform/io/Directory.hpp"
#include "platform/io/File.hpp"
#include <filesystem>

namespace Engine::Platform::IO
{

std::vector<Path> Directory::ScanFiles(Path dirPath)
{
    std::vector<Path> files;
    for(auto& item : std::filesystem::directory_iterator(dirPath.ToString()))
    {
        if(!item.is_directory()) files.push_back(Path(item.path()));
    }
    return files;
}

Directory::Directory(Path path) 
    : Files(ScanFiles(path))
{
    m_path = path;
    m_name = path.GetDirname();
}

Directory::Directory()
{
    m_path = "";
    m_name = "";
}

Directory Directory::GetParentDirectory()
{
    return Directory(m_path.ParentDirectory());
}

std::vector<Directory> Directory::GetSubdirectories()
{
    std::vector<Directory> dirs;
    for(auto& item : std::filesystem::directory_iterator(m_path.ToString()))
    {
        if(item.is_directory())
        {
            dirs.push_back(Directory(item.path()));
        }
    }
    return dirs;
}

std::string Directory::GetName()
{
    return m_name;
}

Path Directory::GetPath()
{
    return m_path;
}

} // namespace Engine::Platform::IO

