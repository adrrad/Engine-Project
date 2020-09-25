#pragma once


#include "File.hpp"

#include <string>
#include <filesystem>

namespace Engine::Platform::IO
{

class Path
{
private:
    std::filesystem::path m_path;

public:

    inline Path(std::filesystem::path cppPath)
    {
        m_path = cppPath;
    }

    inline Path(std::string pathString)
    {
        m_path = std::filesystem::path(pathString);
    }

    inline Path ParentDirectory()
    {
        return m_path.parent_path();
    }

    Path Subdirectory(std::string folderName);

    inline std::string ToString()
    {
        return m_path.string();
    }

};

class Directory
{


public:
    std::string Path;


};

} // namespace Engine::Platform::IO

