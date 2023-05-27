#pragma once
#include "rendering/Texture.hpp"

#include <string>


namespace Utilities
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

    constexpr size_t CalcHash(const char* val)
    {
        static_assert(sizeof(size_t) == 8);
        // FNV-1a 64 bit algorithm
        size_t result = 0xcbf29ce484222325; // FNV offset basis
        size_t i = 0;
        while (val[i] != '\0') 
        {
            result ^= val[i];
            result *= 1099511628211; // FNV prime
            i++;
        }

        return result;
    }

} // namespace Utilities
