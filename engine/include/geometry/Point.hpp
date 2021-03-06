#pragma once

#include <glm/glm.hpp>

namespace Engine::Geometry
{

class Point
{
private:
    
public:
    const float x, y, z;

    Point(glm::vec3 coordinates) : x(coordinates.x), y(coordinates.y), z(coordinates.z) {}
};

} // namespace Engine::Geometry
