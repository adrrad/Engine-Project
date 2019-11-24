#pragma once


#include <string>


namespace Utilities
{
    std::string GetProjectDirectoryPath();
    
    std::string GetResourcesDirectoryPath();

    std::string GetAbsoluteResourcesPath(std::string relativePath);

    std::string GLenumUniformTypeToString(uint32_t type);

    uint32_t CreateHieghtMap(uint32_t width, uint32_t height);

} // namespace Utilities
