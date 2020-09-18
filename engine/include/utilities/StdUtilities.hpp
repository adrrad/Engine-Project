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


}