#include "platform/io/Directory.hpp"
#include "platform/io/Filesystem.hpp"
#include "platform/io/File.hpp"

#include <filesystem>

namespace Platform::IO
{

Directory::Directory(Filesystem* fs, Path path)
{
    if(path.IsAbsolute()) path = fs->GetRelativePath(path);
    m_path = path;
    m_name = path.GetDirname();
    m_parentFilesystem = fs;
}

Directory::Directory(Path path)
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
    auto abs = GetAbsolutePath();
    auto parent = abs.ParentDirectory();
    if(m_parentFilesystem) return Directory(m_parentFilesystem, parent);
    return Directory(m_path.ParentDirectory());
}

std::vector<Directory> Directory::GetSubdirectories()
{
    std::vector<Directory> dirs;
    Path absolutePath = GetAbsolutePath();
    for(auto& item : std::filesystem::directory_iterator(absolutePath.ToString()))
    {
        if(item.is_directory())
        {
            if(m_parentFilesystem) dirs.push_back(Directory(m_parentFilesystem, item.path()));
            else dirs.push_back(Directory(item.path()));
        }
    }
    return dirs;
}

std::vector<Path> Directory::GetFiles()
{
    std::vector<Path> files;
    Path absPath = GetAbsolutePath();
    for(auto item : std::filesystem::directory_iterator(absPath.ToString()))
    {
        if(!item.is_directory())
        {
            Path p = Path(item.path());
            if(m_parentFilesystem) files.push_back(Path(m_parentFilesystem->GetRelativePath(p)));
            else files.push_back(p);
        } 
    }
    return files;
}

std::string Directory::GetName()
{
    return m_name;
}

Path Directory::GetPath()
{
    return m_path;
}

Path Directory::GetAbsolutePath()
{
    if(m_parentFilesystem) return m_parentFilesystem->GetAbsolutePath(m_path);
    return m_path;
}

} // namespace Platform::IO

