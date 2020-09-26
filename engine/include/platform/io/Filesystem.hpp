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
public:
    Filesystem(Path root);

};


} // namespace Engine::Platform::IO

