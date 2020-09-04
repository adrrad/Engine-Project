#include "geometry/Frustum.hpp"

#include "geometry/Sphere.hpp"
#include "geometry/AxisAlignedBox.hpp"

using namespace glm;

namespace Engine::Geometry
{

bool Frustum::IntersectsAxisAlignedBox(AxisAlignedBox* other)
{
    //https://www.gamedev.net/tutorials/programming/general-and-gameplay-programming/frustum-culling-r4613/
    const vec3& min = other->Min;
    const vec3& max = other->Max;
    for(int i = 0; i < 6; i++)
    {
        if (Planes[i].x * min.x + Planes[i].y * max.y + Planes[i].z * min.z + Planes[i].w > 0) continue; 
        if (Planes[i].x * min.x + Planes[i].y * max.y + Planes[i].z * max.z + Planes[i].w > 0) continue; 
        if (Planes[i].x * max.x + Planes[i].y * max.y + Planes[i].z * max.z + Planes[i].w > 0) continue; 
        if (Planes[i].x * max.x + Planes[i].y * max.y + Planes[i].z * min.z + Planes[i].w > 0) continue;
        if (Planes[i].x * max.x + Planes[i].y * min.y + Planes[i].z * min.z + Planes[i].w > 0) continue; 
        if (Planes[i].x * max.x + Planes[i].y * min.y + Planes[i].z * max.z + Planes[i].w > 0) continue; 
        if (Planes[i].x * min.x + Planes[i].y * min.y + Planes[i].z * max.z + Planes[i].w > 0) continue; 
        if (Planes[i].x * min.x + Planes[i].y * min.y + Planes[i].z * min.z + Planes[i].w > 0) continue;
        return false;
    }
    return true;  
}

bool Frustum::IntersectsSphere(Sphere* other)
{
    const vec3& pos = other->Center;
    const float& r = other->Radius;
    for(int i = 0; i < 6; i++)
    {
        float dist = Planes[i].x * pos[i] + Planes[i].y * pos.y + Planes[i].z * pos.z + Planes[i].w;
        if(dist <= -r) return false;
    }
    return true;   
}

Volume* Frustum::GetTransformed(glm::mat4 trs)
{
    throw "Not implemented";
}


} // namespace Engine::Geometry