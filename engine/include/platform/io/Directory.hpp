#pragma once


#include "File.hpp"
#include "Path.hpp"

#include <string>
#include <vector>
#include <filesystem>

namespace Engine::Platform::IO
{

class Directory
{


public:
    const std::string Path;
    const std::vector<File> Files;

    Directory(Path path, std::vector<File> files) : Path(path), Files(files)
    {

    }

};

} // namespace Engine::Platform::IO

