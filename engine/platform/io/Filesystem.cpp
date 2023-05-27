#include "platform/io/Filesystem.hpp"

#include <queue>

#include <future>

namespace Platform::IO
{


Filesystem::Filesystem(Path root) : m_rootDir(Directory(this, root)), m_root(root)
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
        auto files = dir.GetFiles();
        for(auto& file : files)
        {
            action(file);
        }
    }
}

} // namespace Platform::IO
