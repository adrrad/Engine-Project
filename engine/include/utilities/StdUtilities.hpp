#pragma once

#include "rendering/RenderingTypedefs.hpp"

#include <vector>
#include <algorithm>
#include <string>

namespace Engine::Utilities
{   
    template<typename T>
    int IndexOf(std::vector<T> v, T& element)
    {
        auto it = std::find(v.begin(), v.end(), element);
        if(it != v.end()) return std::distance(v.begin(), it);
        return -1;
    }

    inline std::vector<std::string> Split(const std::string& str, std::string delimiter)
    {
        std::vector<std::string> out;
        std::string text = str;
        int pos = 0;
        while((pos = text.find(delimiter)) != std::string::npos)
        {
            out.push_back(text.substr(0, pos));
            text.erase(0, pos + delimiter.length());
        }
        return out;
    }

    inline bool Contains(const std::string& str, std::string substring)
    {;
        return str.find(substring) != std::string::npos;
    }

}