#pragma once


#include "platform/io/Path.hpp"
#include "platform/io/File.hpp"
#include "platform/io/Directory.hpp"

#include <string>
#include <filesystem>

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


    inline std::vector<Engine::Platform::IO::File *>::iterator begin() noexcept { return m_files.begin(); }
    inline std::vector<Engine::Platform::IO::File *>::const_iterator cbegin() const noexcept { return m_files.cbegin(); }
    inline std::vector<Engine::Platform::IO::File *>::iterator end() noexcept { return m_files.end(); }
    inline std::vector<Engine::Platform::IO::File *>::const_iterator cend() const noexcept { return m_files.cend(); }
};


} // namespace Engine::Platform::IO

