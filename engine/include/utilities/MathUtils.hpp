#pragma once

#include <glm/glm.hpp>

namespace Utilities
{
    uint32_t RoundUp(uint32_t value, uint32_t multiple);

    inline float Min(float a, float b)
    {
        return a < b ? a : b;
    }

    inline float Max(float a, float b)
    {
        return a > b ? a : b;
    }

    /**
     * @brief Calculates a new vector with the minimum components from vectors a and b.
     * 
     * @param a 3D vector.
     * @param b 3D vector.
     * @return glm::vec3 Vector with minimum components from both input vectors.
     */
    inline glm::vec3 Min(glm::vec3 a, glm::vec3 b)
    {
        glm::vec3 out;
        out.x = Utilities::Min(a.x, b.x);
        out.y = Utilities::Min(a.y, b.y);
        out.z = Utilities::Min(a.z, b.z);
        return out;
    }

    /**
     * @brief Calculates a new vector with the maximum components from vectors a and b.
     * 
     * @param a 3D vector.
     * @param b 3D vector.
     * @return glm::vec3 Vector with maximum components from both input vectors.
     */
    inline glm::vec3 Max(glm::vec3 a, glm::vec3 b)
    {
        glm::vec3 out;
        out.x = Utilities::Max(a.x, b.x);
        out.y = Utilities::Max(a.y, b.y);
        out.z = Utilities::Max(a.z, b.z);
        return out;
    }

} // namespace Utilities
