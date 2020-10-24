#pragma once


#include "File.hpp"
#include "Path.hpp"

#include <vector>

namespace Engine::Platform::IO
{
class Filesystem;

class Directory
{
friend class Filesystem;

    Path m_path;

    std::string m_name;

    Filesystem* m_parentFilesystem = nullptr;

    Directory(Filesystem* fs, Path path);
public:

    Directory();

    Directory(Path path);

    Directory GetParentDirectory();

    std::vector<Directory> GetSubdirectories();

    std::vector<Path> GetFiles();
    
    std::string GetName();

    Path GetPath();

    Path GetAbsolutePath();

};

} // namespace Engine::Platform::IO

