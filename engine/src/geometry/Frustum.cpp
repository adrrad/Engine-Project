#include "geometry/Frustum.hpp"

#include "geometry/Sphere.hpp"
#include "geometry/AxisAlignedBox.hpp"

#include "utilities/Printing.hpp"

using namespace glm;

namespace Engine::Geometry
{

Frustum::Frustum(glm::mat4 m)
{
    //https://www.reddit.com/r/gamedev/comments/xj47t/does_glm_support_frustum_plane_extraction/
    Planes[0].x = m[0][3] + m[0][0];
    Planes[0].y = m[1][3] + m[1][0];
    Planes[0].z = m[2][3] + m[2][0];
    Planes[0].w = m[3][3] + m[3][0];

    Planes[1].x = m[0][3] - m[0][0];
    Planes[1].y = m[1][3] - m[1][0];
    Planes[1].z = m[2][3] - m[2][0];
    Planes[1].w = m[3][3] - m[3][0];

    Planes[2].x = m[0][3] - m[0][1];
    Planes[2].y = m[1][3] - m[1][1];
    Planes[2].z = m[2][3] - m[2][1];
    Planes[2].w = m[3][3] - m[3][1];

    Planes[3].x = m[0][3] + m[0][1];
    Planes[3].y = m[1][3] + m[1][1];
    Planes[3].z = m[2][3] + m[2][1];
    Planes[3].w = m[3][3] + m[3][1];

    Planes[4].x = m[0][2];
    Planes[4].y = m[1][2];
    Planes[4].z = m[2][2];
    Planes[4].w = m[3][2];

    Planes[5].x = m[0][3] - m[0][2];
    Planes[5].y = m[1][3] - m[1][2];
    Planes[5].z = m[2][3] - m[2][2];
    Planes[5].w = m[3][3] - m[3][2];
}

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