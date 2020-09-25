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

    inline Path(const Path &other)
    {
        m_path = std::filesystem::path(other.ToString());
    }

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

    inline Path Subdirectory(std::string folderName)
    {
        return m_path.string() + "/" + folderName;
    }

    inline std::string ToString() const
    {
        return m_path.string();
    }

};


} // namespace Engine::Platform::IO

