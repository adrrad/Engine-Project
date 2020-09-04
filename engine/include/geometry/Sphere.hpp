#pragma once
#include "geometry/Volume.hpp"

#include <glm/glm.hpp>

namespace Engine::Geometry
{

class Sphere : public Volume
{
private:

public:
    const glm::vec3 Center;
    const float Radius;

    Sphere(glm::vec3 center, float radius);

    bool IntersectsAxisAlignedBox(AxisAlignedBox* other) override;

    bool IntersectsSphere(Sphere* other) override;

    Volume* GetTransformed(glm::mat4 trs) override;
    
};

} // namespace Engine::Geometry