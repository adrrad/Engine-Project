#include "utilities/Utilities.hpp"

#include <filesystem>

namespace fs = std::filesystem;

namespace Utilities
{
    std::string GetProjectDirectoryPath()
    {
        return fs::current_path().string();
    }
    
    std::string GetResourcesDirectoryPath()
    {
        return GetProjectDirectoryPath() + std::string("\\resources");
    }

    std::string GetAbsoluteResourcesPath(std::string relativePath)
    {
        return GetResourcesDirectoryPath() + relativePath;
    }
} // namespace Utilities