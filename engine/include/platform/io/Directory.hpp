#pragma once


#include "File.hpp"
#include "Path.hpp"

#include <vector>

namespace Engine::Platform::IO
{

class Directory
{

    static std::vector<File> ScanFiles(Path dirPath);
    static std::vector<Directory> ScanDirectories(Path dirPath, bool recursive);

    Directory(Path path);

public:
    const Path DirectoryPath;
    const std::string Name;
    std::vector<File> Files;
    std::vector<Directory> Subdirectories;
    Directory(Path path, bool scanRecursively);



};

} // namespace Engine::Platform::IO

