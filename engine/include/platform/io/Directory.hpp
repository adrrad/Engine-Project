#pragma once


#include "File.hpp"
#include "Path.hpp"

#include <vector>

namespace Engine::Platform::IO
{

class Directory
{

    static std::vector<Path> ScanFiles(Path dirPath);
    // static std::vector<Directory> ScanDirectories(Path dirPath);


public:
    Path DirectoryPath;
    std::string Name;
    std::vector<Path> Files;
    // std::vector<Directory> Subdirectories;

    Directory();

    Directory(Path path);

    Directory GetParentDirectory();

    std::vector<Directory> GetSubdirectories();

};

} // namespace Engine::Platform::IO

