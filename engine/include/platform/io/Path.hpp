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

    inline Path()
    {

    }

    inline Path(const Path &other)
    {
        m_path = std::filesystem::path(Utilities::Replace(other.ToString(), "\\", "/"));
    }

    inline Path(std::filesystem::path cppPath) : Path(cppPath.string())
    {

    }

    inline Path(std::string pathString)
    {
        std::string str = Utilities::Replace(pathString, "\\", "/");
        if(std::filesystem::is_directory(str) && str.back() != '/')
        {
            str += "/";
        } 
        m_path = str;
    }

    inline Path(const char* pathString)
    {
        m_path = std::filesystem::path(pathString);
        m_path.make_preferred();
    }

    inline Path ParentDirectory() const
    {
        return std::filesystem::canonical(m_path).parent_path();
    }

    inline bool Exists() const
    {
        return std::filesystem::exists(m_path);
    }

    inline bool IsDirectory() const
    {
        return std::filesystem::is_directory(m_path);
    }

    inline bool IsAbsolute() const
    {
        return m_path.is_absolute();
    }

    inline bool IsRelative() const
    {
        return m_path.is_relative();
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

    inline bool operator==(const Path& other) const
    {
        return ToString() == other.ToString();
    }

    inline Path operator+(const Path& other) const
    {
        return std::filesystem::canonical(m_path / other.m_path);
    }
};


} // namespace Engine::Platform::IO

