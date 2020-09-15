#pragma once

#include <glm/glm.hpp>

namespace Utilities
{
    uint32_t RoundUp(uint32_t value, uint32_t multiple);

    inline bool ApproximatelyEqual(float a, float b, float epsilon = 0.00001f)
    {
        return abs(a-b) < epsilon;
    }

    inline bool ApproximatelyEqual(glm::vec3 a, glm::vec3 b, float epsilon = 0.00001f)
    {
        return length(abs(a-b)) < epsilon;
    }

    inline bool ApproximatelyEqual(glm::vec4 a, glm::vec4 b, float epsilon = 0.00001f)
    {
        return length(abs(a-b)) < epsilon;
    }

    inline float Min(float a, float b)
    {
        return a < b ? a : b;
    }

    inline float Max(float a, float b)
    {
        return a > b ? a : b;
    }

    inline uint32_t Min(uint32_t a, uint32_t b)
    {
        return a < b ? a : b;
    }

    inline uint32_t Max(uint32_t a, uint32_t b)
    {
        return a > b ? a : b;
    }
    
    inline int Min(int a, int b)
    {
        return a < b ? a : b;
    }

    inline int Max(int a, int b)
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

    /**
     * @brief Calculates a new vector with the minimum components from vectors a and b.
     * 
     * @param a 2D vector.
     * @param b 2D vector.
     * @return glm::vec2 Vector with minimum components from both input vectors.
     */
    inline glm::vec2 Min(glm::vec2 a, glm::vec2 b)
    {
        glm::vec2 out;
        out.x = Utilities::Min(a.x, b.x);
        out.y = Utilities::Min(a.y, b.y);
        return out;
    }

    /**
     * @brief Calculates a new vector with the maximum components from vectors a and b.
     * 
     * @param a 2D vector.
     * @param b 2D vector.
     * @return glm::vec2 Vector with maximum components from both input vectors.
     */
    inline glm::vec2 Max(glm::vec2 a, glm::vec2 b)
    {
        glm::vec2 out;
        out.x = Utilities::Max(a.x, b.x);
        out.y = Utilities::Max(a.y, b.y);
        return out;
    }

} // namespace Utilities
