#pragma once
#include "geometry/Volume.hpp"
#include "geometry/AxisAlignedBox.hpp"
#include <glm/glm.hpp>

namespace Geometry
{

class Frustum : public Volume
{
private:
public:
    glm::vec4 Planes[6];

    Frustum(glm::mat4 viewProjection);

    inline bool IntersectsAxisAlignedBox(AxisAlignedBox* other) override;

    bool IntersectsOrientedBox(OrientedBox* other) override;

    bool IntersectsSphere(Sphere* other) override;

    bool ContainsPoint(const Point& p) override;

    Volume* GetTransformed(glm::mat4 trs) override;
};


bool Frustum::IntersectsAxisAlignedBox(AxisAlignedBox* other)
{
    //https://www.gamedev.net/tutorials/programming/general-and-gameplay-programming/frustum-culling-r4613/
    const glm::vec3& Min = other->Min;
    const glm::vec3& Max = other->Max;
    bool inside = true;
    for(int i = 0; i < 6; i++)
    {
        // if (Planes[i].x * Min.x + Planes[i].y * Max.y + Planes[i].z * Min.z + Planes[i].w > 0) continue; 
        // if (Planes[i].x * Min.x + Planes[i].y * Max.y + Planes[i].z * Max.z + Planes[i].w > 0) continue; 
        // if (Planes[i].x * Max.x + Planes[i].y * Max.y + Planes[i].z * Max.z + Planes[i].w > 0) continue; 
        // if (Planes[i].x * Max.x + Planes[i].y * Max.y + Planes[i].z * Min.z + Planes[i].w > 0) continue;
        // if (Planes[i].x * Max.x + Planes[i].y * Min.y + Planes[i].z * Min.z + Planes[i].w > 0) continue; 
        // if (Planes[i].x * Max.x + Planes[i].y * Min.y + Planes[i].z * Max.z + Planes[i].w > 0) continue; 
        // if (Planes[i].x * Min.x + Planes[i].y * Min.y + Planes[i].z * Max.z + Planes[i].w > 0) continue; 
        // if (Planes[i].x * Min.x + Planes[i].y * Min.y + Planes[i].z * Min.z + Planes[i].w > 0) continue;
        // return false;
        float d = 
            glm::max(Min.x * Planes[i].x, Max.x * Planes[i].x) + 
            glm::max(Min.y * Planes[i].y, Max.y * Planes[i].y) +
            glm::max(Min.z * Planes[i].z, Max.z * Planes[i].z) + 
            Planes[i].w; 
        inside &= d > 0;
    }
    return inside;  
    // return true;
}

} // namespace Geometry
