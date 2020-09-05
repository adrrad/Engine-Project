#pragma once

#include <glm/glm.hpp>

#include <iostream>

namespace Utilities
{

    inline void Print(glm::vec3 v)
    {
        std::cout << "vec3(" << v.x << "," << v.y << "," << v.z << ")" << std::endl;
    }

    inline void Print(glm::vec4 v)
    {
        std::cout << "vec4(" << v.x << "," << v.y << "," << v.z <<  "," << v.w << ")" << std::endl;
    }

    inline void Print(const char* txt)
    {
        std::cout << txt << std::endl;
    }

} // namespace Utilities
