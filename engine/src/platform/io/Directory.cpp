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
        if(item.is_directory()) dirs.push_back(Directory(item.path(), recursive));
    }
    return dirs;
}

Directory::Directory(Path path, bool scanRecursively)
    : DirectoryPath(path), Files(ScanFiles(path)), Subdirectories(ScanDirectories(path, scanRecursively))
{

}


} // namespace Engine::Platform::IO

