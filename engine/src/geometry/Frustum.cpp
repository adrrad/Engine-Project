#include "geometry/Frustum.hpp"

#include "geometry/Sphere.hpp"
#include "geometry/AxisAlignedBox.hpp"

#include "utilities/Printing.hpp"

using namespace glm;

namespace Engine::Geometry
{

bool Frustum::IntersectsAxisAlignedBox(AxisAlignedBox* other)
{
    //https://www.gamedev.net/tutorials/programming/general-and-gameplay-programming/frustum-culling-r4613/
    const vec3& Min = other->Min;
    const vec3& Max = other->Max;
    bool inside = true;
    for(int i = 0; i < 6; i++)
    {
        if (Planes[i].x * Min.x + Planes[i].y * Max.y + Planes[i].z * Min.z + Planes[i].w > 0) continue; 
        if (Planes[i].x * Min.x + Planes[i].y * Max.y + Planes[i].z * Max.z + Planes[i].w > 0) continue; 
        if (Planes[i].x * Max.x + Planes[i].y * Max.y + Planes[i].z * Max.z + Planes[i].w > 0) continue; 
        if (Planes[i].x * Max.x + Planes[i].y * Max.y + Planes[i].z * Min.z + Planes[i].w > 0) continue;
        if (Planes[i].x * Max.x + Planes[i].y * Min.y + Planes[i].z * Min.z + Planes[i].w > 0) continue; 
        if (Planes[i].x * Max.x + Planes[i].y * Min.y + Planes[i].z * Max.z + Planes[i].w > 0) continue; 
        if (Planes[i].x * Min.x + Planes[i].y * Min.y + Planes[i].z * Max.z + Planes[i].w > 0) continue; 
        if (Planes[i].x * Min.x + Planes[i].y * Min.y + Planes[i].z * Min.z + Planes[i].w > 0) continue;
        return false;
        // float d = 
        //     max(Min.x * Planes[i].x, Max.x * Planes[i].x) + 
        //     max(Min.y * Planes[i].y, Max.y * Planes[i].y) +
        //     max(Min.z * Planes[i].z, Max.z * Planes[i].z) + 
        //     Planes[i].w; 
        // inside &= d > 0;
    }
    // return inside;  
    return true;
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

Volume* Frustum::GetTransformed(glm::mat4 trs)
{
    throw "Not implemented";
}


} // namespace Engine::Geometry