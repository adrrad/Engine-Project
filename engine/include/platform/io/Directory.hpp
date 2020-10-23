#pragma once


#include "File.hpp"
#include "Path.hpp"

#include <vector>

namespace Engine::Platform::IO
{

class Directory
{

    static std::vector<Path> ScanFiles(Path dirPath);

    Path m_path;

    std::string m_name;

public:
    std::vector<Path> Files;
    // std::vector<Directory> Subdirectories;

    Directory();

    Directory(Path path);

    Directory GetParentDirectory();

    std::vector<Directory> GetSubdirectories();
    
    std::string GetName();

    Path GetPath();

};

} // namespace Engine::Platform::IO

