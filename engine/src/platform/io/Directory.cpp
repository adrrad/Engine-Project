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
    : DirectoryPath(path), Name(path.GetDirname()), Files(ScanFiles(path))
{

}

Directory::Directory() : DirectoryPath(""), Name("")
{

}

Directory Directory::GetParentDirectory()
{
    return Directory(DirectoryPath.ParentDirectory());
}

std::vector<Directory> Directory::GetSubdirectories()
{
    std::vector<Directory> dirs;
    for(auto& item : std::filesystem::directory_iterator(DirectoryPath.ToString()))
    {
        if(item.is_directory())
        {
            dirs.push_back(Directory(item.path()));
        }
    }
    return dirs;
}

} // namespace Engine::Platform::IO

