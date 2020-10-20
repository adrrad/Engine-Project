#include "platform/io/Directory.hpp"
#include "platform/io/File.hpp"
#include <filesystem>

namespace Engine::Platform::IO
{

std::vector<File> Directory::ScanFiles(Path dirPath)
{
    std::vector<File> files;
    for(auto& item : std::filesystem::directory_iterator(dirPath.ToString()))
    {
        if(!item.is_directory()) files.push_back(File(item.path()));
    }
    return files;
}

std::vector<Directory> Directory::ScanDirectories(Path dirPath, bool recursive)
{
    std::vector<Directory> dirs;
    for(auto& item : std::filesystem::directory_iterator(dirPath.ToString()))
    {
        if(item.is_directory())
        {
            if(recursive) dirs.push_back(Directory(item.path(), recursive));
            else dirs.push_back(Directory(item.path()));
        }
    }
    return dirs;
}

Directory::Directory() : DirectoryPath(""), Name("")
{

}

Directory::Directory(Path path, bool scanRecursively)
    : DirectoryPath(path), Name(path.GetDirname()), Files(ScanFiles(path)), Subdirectories(ScanDirectories(path, scanRecursively))
{

}

Directory Directory::GetParentDirectory()
{
    return Directory(DirectoryPath.ParentDirectory(), false);
}


Directory::Directory(Path path) 
    : DirectoryPath(path), Name(path.GetDirname()), Files(ScanFiles(path)), Subdirectories({})
{

}

} // namespace Engine::Platform::IO

