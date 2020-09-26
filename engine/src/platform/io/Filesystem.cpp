#include "platform/io/Filesystem.hpp"

#include <queue>

namespace Engine::Platform::IO
{

void Filesystem::ScanAllFiles()
{
    std::queue<Directory*> q;
    q.push(&m_root);
    while(!q.empty())
    {
        Directory& dir = *q.front(); q.pop();
        for(auto& subdir : dir.Subdirectories) q.push(&subdir);
        for(auto& file : dir.Files) m_files.push_back(&file);
    }
}

Filesystem::Filesystem(Path root) : m_root(Directory(root, true))
{
    ScanAllFiles();
}

} // namespace Engine::Platform::IO
