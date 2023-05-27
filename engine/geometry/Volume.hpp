#pragma once

#include <glm/glm.hpp>

namespace Geometry
{

//FORWARD DECLARATIONS
class AxisAlignedBox;
class OrientedBox;
class Sphere;
class Frustum;
class Point;


class Volume
{

private:
    
public:
    virtual bool IntersectsAxisAlignedBox(AxisAlignedBox* other) = 0;
    virtual bool IntersectsOrientedBox(OrientedBox* other) = 0;
    virtual bool IntersectsSphere(Sphere* other) = 0;
    virtual bool ContainsPoint(const Point& p) = 0;
    
    virtual Volume* GetTransformed(glm::mat4 trs) = 0;

};

} // namespace Geometry
