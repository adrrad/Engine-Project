#include "geometry/Frustum.hpp"

#include "geometry/Sphere.hpp"
#include "geometry/AxisAlignedBox.hpp"

#include "utilities/Printing.hpp"

using namespace glm;

namespace Engine::Geometry
{

bool Frustum::IntersectsSphere(Sphere* other)
{
    const vec3& pos = other->Center;
    const float& r = other->Radius;
    for(int i = 0; i < 6; i++)
    {
        float dist = Planes[i].x * pos.x + Planes[i].y * pos.y + Planes[i].z * pos.z + Planes[i].w;
        if(dist <= -r) return false;
    }
    return true;   
}

bool Frustum::ContainsPoint(const Point& p)
{
    throw "Not implemented";
}

Volume* Frustum::GetTransformed(glm::mat4 trs)
{
    throw "Not implemented";
}


} // namespace Engine::Geometry