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


public:
    const Path DirectoryPath;
    const std::vector<File> Files;
    const std::vector<Directory> Subdirectories;

    Directory(Path path, bool scanRecursively = false);

};

} // namespace Engine::Platform::IO

