#pragma once

#include "utilities/StringUtilities.hpp"

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
        m_path.make_preferred();
    }

    inline Path(std::string pathString)
    {
        m_path = std::filesystem::path(pathString);
        m_path.make_preferred();
    }

    inline Path(const char* pathString)
    {
        m_path = std::filesystem::path(pathString);
        m_path.make_preferred();
    }

    inline Path ParentDirectory() const
    {
        return m_path.parent_path();
    }

    inline Path Subdirectory(std::string folderName)
    {
        return m_path.string() + "/" + folderName;
    }

    inline bool IsDirectory() const
    {
        return std::filesystem::is_directory(m_path);
    }

    inline std::string GetFilename() const
    {
        return m_path.filename().string();
    }

    inline std::string GetDirname() const
    {
        return Utilities::Split(ToString(), "/").back();
    }

    inline std::string GetExtension() const
    {
        return m_path.extension().string();
    }

    inline std::string ToString() const
    {
        return Utilities::Replace(m_path.string(), "\\", "/");
    }

    inline bool operator==(const Path& other)
    {
        return ToString() == other.ToString();
    }

};


} // namespace Engine::Platform::IO

