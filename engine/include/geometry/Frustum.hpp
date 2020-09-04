#pragma once
#include "geometry/Volume.hpp"

#include <glm/glm.hpp>

namespace Engine::Geometry
{

class Frustum : public Volume
{
private:
public:
    glm::vec4 Planes[6];

    bool IntersectsAxisAlignedBox(AxisAlignedBox* other) override;

    bool IntersectsSphere(Sphere* other) override;

    Volume* GetTransformed(glm::mat4 trs) override;
};

} // namespace Engine::Geometry
