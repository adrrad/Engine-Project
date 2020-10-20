#pragma once


#include "platform/io/Path.hpp"
#include "platform/io/File.hpp"
#include "platform/io/Directory.hpp"

#include "Exceptions.hpp"

#include <string>
#include <filesystem>
#include <fstream>

namespace Engine::Platform::IO
{

class Filesystem
{
private:
    Directory m_root;
    std::vector<File*> m_files;

    void ScanAllFiles();

public:

    Filesystem(Path root);

    inline Directory* GetRootDirectory() { return &m_root; }

    inline Path GetRootPath() { return m_root.DirectoryPath; }

    inline Path GetRelativePath(const Path& absolutePath)
    { 
        std::filesystem::path abs  = absolutePath.ToString();
        std::filesystem::path base = m_root.DirectoryPath.ToString(); 
        return std::filesystem::relative(abs, base);
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
        m_files.push_back(file);
        file->Open(File::WRITE | File::TRUNCATE);
        file->Close();
        return file;
    }

    inline bool FileExists(const Path& filePath)
    {
        return std::filesystem::exists(filePath.ToString());
    }

    inline File* GetFile(const Path& filePath)
    {
        for(auto file : m_files)
        {
            if(file->FilePath.ToString() == filePath.ToString()) return file;
        }
        throw FileNotFoundException("Could not find file: " + filePath.ToString());
    }

    inline Directory GetDirectory(const Path& realtiveDirPath)
    {
        return Directory(m_root.DirectoryPath.ToString()+realtiveDirPath.ToString(), false);
    }

    inline std::vector<Engine::Platform::IO::File *>::iterator begin() noexcept { return m_files.begin(); }
    inline std::vector<Engine::Platform::IO::File *>::const_iterator cbegin() const noexcept { return m_files.cbegin(); }
    inline std::vector<Engine::Platform::IO::File *>::iterator end() noexcept { return m_files.end(); }
    inline std::vector<Engine::Platform::IO::File *>::const_iterator cend() const noexcept { return m_files.cend(); }
};


} // namespace Engine::Platform::IO

