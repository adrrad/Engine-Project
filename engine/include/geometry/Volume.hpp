#pragma once

#include <glm/glm.hpp>

namespace Engine::Geometry
{

class AxisAlignedBox;
class Sphere;

class Volume
{

private:
    
public:
    virtual bool IntersectsAxisAlignedBox(AxisAlignedBox* other) = 0;
    virtual bool IntersectsSphere(Sphere* other) = 0;
    virtual Volume* GetTransformed(glm::mat4 trs) = 0;

};

} // namespace Engine::Geometry
