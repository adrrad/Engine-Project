#pragma once
#include "rendering/Texture.hpp"

#include <string>


namespace Engine::Utilities
{
    std::string GetProjectDirectoryPath();
    
    std::string GetResourcesDirectoryPath();

    std::string GetAbsoluteResourcesPath(std::string relativePath);

    std::string GLenumUniformTypeToString(uint32_t type);

    std::string ReadFile(std::string path);

    void SaveToTextFile(std::string content, std::string fileNamfilePathe);

    Rendering::Texture* ImportTexture(std::string abosultePath);

    Rendering::Texture* ImportTexture(std::string abosultePath, uint32_t glTarget);

} // namespace Engine::Utilities
