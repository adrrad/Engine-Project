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

    uint32_t CreateHieghtMap(uint32_t width, uint32_t height);

    Rendering::Texture* ImportTexture(std::string abosultePath);

    Rendering::Texture* ImportTexture(std::string abosultePath, uint32_t glTarget);

} // namespace Engine::Utilities
