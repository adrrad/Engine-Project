#include "platform/io/Filesystem.hpp"

#include <queue>

namespace Engine::Platform::IO
{


Filesystem::Filesystem(Path root) : m_rootDir(Directory(root)), m_root(root)
{
    
}

void Filesystem::ForEachFile(std::function<void(Path)> action)
{
    std::queue<Directory> q;
    q.push(m_rootDir);
    while(!q.empty())
    {
        Directory dir = q.front(); q.pop();
        auto subdirs = dir.GetSubdirectories();
        for(auto& subdir : subdirs) q.push(subdir);
        for(auto& file : dir.Files) action(file);
    }
}

} // namespace Engine::Platform::IO
