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

    template<typename T>
    int IndexOf(std::vector<T> v, T& element)
    {
        auto it = std::find(v.begin(), v.end(), element);
        if(it != v.end()) return std::distance(v.begin(), it);
        return -1;
    }

} // namespace Engine::Utilities
