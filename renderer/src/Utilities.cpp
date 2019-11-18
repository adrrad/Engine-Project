#include "utilities/Utilities.hpp"

#include <filesystem>

namespace fs = std::filesystem;

namespace Utilities
{
    std::string GetProjectDirectoryPath()
    {
        return fs::current_path().string();
    }
    
    std::string GetAppDirectoryPath()
    {
        return GetProjectDirectoryPath() + std::string("\\app");
    }

    std::string GetAbosoluteAppFilePath(std::string relativeAppFilePath)
    {
        return GetAppDirectoryPath() + relativeAppFilePath;
    }
} // namespace Utilities