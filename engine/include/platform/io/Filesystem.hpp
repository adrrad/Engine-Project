#pragma once


#include "platform/io/Path.hpp"
#include "platform/io/File.hpp"
#include "platform/io/Directory.hpp"

#include "Exceptions.hpp"

#include <string>
#include <fstream>
#include <functional>
#include <filesystem>
#include <unordered_map>

namespace Engine::Platform::IO
{

class Filesystem
{
private:
    Path m_root;
    Directory m_rootDir;
    std::unordered_map<std::string, File*> m_activeFiles;

public:

    Filesystem(Path root);

    inline Directory* GetRootDirectory() { return &m_rootDir; }

    inline Path GetRootPath() { return m_root; }

    inline Path GetRelativePath(const Path& absolutePath)
    { 
        std::filesystem::path abs  = absolutePath.ToString();
        std::filesystem::path base = m_root.ToString(); 
        return std::filesystem::relative(abs, base);
    }

    inline Path GetAbsolutePath(const Path& relativePath)
    {
        if(relativePath.IsAbsolute()) return relativePath;
        return m_root + "/" + relativePath;
    }

    inline void CreateDirectory(const Path& relativePath)
    {
        std::filesystem::path abspath = GetRootPath().ToString() + relativePath.ToString();
        std::filesystem::create_directory(abspath);
    }

    inline File* CreateFile(const Path& relativePath)
    {
        std::filesystem::path abspath = GetRootPath().ToString() + relativePath.ToString();
        File* file = new File(abspath);
        file->Open(File::WRITE | File::TRUNCATE);
        file->Close();
        return file;
    }

    inline bool FileExists(const Path& filePath)
    {
        return std::filesystem::exists(filePath.ToString());
    }

    inline File* GetFile(const Path& relativePath)
    {
        Path absolutePath = GetAbsolutePath(relativePath.ToString());
        if(absolutePath.Exists() && !absolutePath.IsDirectory())
        {
            if(m_activeFiles.contains(absolutePath.ToString())) 
            {
                return m_activeFiles[absolutePath.ToString()];
            }
            else
            {
                File* file = new File(this, relativePath);
                m_activeFiles[absolutePath.ToString()] = file;
                return file;
            }
        }
        throw IOException("Path '" + relativePath.ToString() + "' does not point to an existing file!");
    }

    inline Directory GetDirectory(const Path& relativePath)
    {
        return Directory(m_rootDir.GetPath().ToString()+relativePath.ToString());
    }

    void ForEachFile(std::function<void(Path)> action);
};


} // namespace Engine::Platform::IO

