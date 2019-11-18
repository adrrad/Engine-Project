#pragma once


#include <string>


namespace Utilities
{
    std::string GetProjectDirectoryPath();
    
    std::string GetAppDirectoryPath();

    std::string GetAbosoluteAppFilePath(std::string relativeAppFilePath);
} // namespace Utilities
