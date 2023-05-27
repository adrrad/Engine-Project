#include "geometry/OrientedBox.hpp"

#include "geometry/AxisAlignedBox.hpp"
#include "Debug.hpp"

#include <limits>

using namespace std;
using namespace glm;

namespace Geometry
{

bool OrientedBox::Separated(vector<vec3> vsA, vector<vec3> vsB, vec3 axis)
{
    if(vsA.size() != 8 || vsB.size() != 8) AssertFail("Oriented bounding boxes must have exactly 8 vertices each!");
    if(axis == vec3(0,0,0)) return false;

    float aMin = std::numeric_limits<float>::max();
    float aMax = std::numeric_limits<float>::min();
    float bMin = aMin;
    float bMax = aMax;

    for(int index = 0; index < 8; index++)
    {
        float aDist = glm::dot(vsA[index], axis);
        float bDist = glm::dot(vsB[index], axis);
        aMin = std::min(aMin, aDist);
        aMax = std::max(aMax, aDist);
        bMin = std::min(bMin, bDist);
        bMax = std::max(bMax, bDist);
    }
    float longSpan = std::max(aMax, bMax) - std::min(aMin, bMin);
    float sumSpan = aMax - aMin + bMax - bMin;
    return longSpan >= sumSpan;
}

void OrientedBox::CalculateCorners()
{
    vec3 c = Center;
    vec3 u = U * Hu;
    vec3 v = V * Hv;
    vec3 w = W * Hw;
    m_corners.push_back(c - u - v - w);
    m_corners.push_back(c - u + v - w);
    m_corners.push_back(c - u + v + w);
    m_corners.push_back(c - u - v + w);

    m_corners.push_back(c + u - v - w);
    m_corners.push_back(c + u + v - w);
    m_corners.push_back(c + u + v + w);
    m_corners.push_back(c + u - v + w);
}

OrientedBox::OrientedBox(vec3 center, vec3 u, vec3 v, vec3 w, float hu, float hv, float hw)
{
    Center = center;
    U = u;
    V = v;
    W = w;
    Hu = hu;
    Hv = hv;
    Hw = hw;
}

OrientedBox::OrientedBox(const AxisAlignedBox &aab)
{
    vec3 diagonal = aab.Max - aab.Min;
    Center = aab.Min + diagonal * 0.5f;
    U = {0, 0, 1};
    V = {1, 0, 0};
    W = {0, 1, 0};
    Hu = diagonal.z * 0.5f;
    Hv = diagonal.x * 0.5f;
    Hw = diagonal.y * 0.5f;
}

OrientedBox::OrientedBox(const AxisAlignedBox &aab, glm::mat4 trs) : OrientedBox(aab)
{
    Center = vec3(trs * vec4(Center, 1.0f));
    U = vec3(trs * vec4(U, 0.0f));
    V = vec3(trs * vec4(V, 0.0f));
    W = vec3(trs * vec4(W, 0.0f));
}

bool OrientedBox::IntersectsAxisAlignedBox(AxisAlignedBox* other) 
{
    vec3 bU = {0,0,1};
    vec3 bV = {1,0,0};
    vec3 bW = {0,1,0};
    vec3 bmin = other->Min;
    vec3 bmax = other->Max;
    vector<vec3> cornersA = GetCorners();
    vector<vec3> cornersB = {
        bmin, {bmax.x, bmin.y, bmin.z}, {bmin.x, bmax.y, bmin.z}, {bmax.x, bmax.y, bmin.z},
        {bmax.x, bmin.y, bmax.z}, {bmin.x, bmax.y, bmax.z}, {bmin.x, bmin.y, bmax.z}, bmax
    };


    if(Separated(cornersA, cornersB, U)) return false;
    if(Separated(cornersA, cornersB, V)) return false;
    if(Separated(cornersA, cornersB, W)) return false;

    if(Separated(cornersA, cornersB, bU)) return false;
    if(Separated(cornersA, cornersB, bV)) return false;
    if(Separated(cornersA, cornersB, bW)) return false;

    if(Separated(cornersA, cornersB, cross(U,bU))) return false;
    if(Separated(cornersA, cornersB, cross(U,bV))) return false;
    if(Separated(cornersA, cornersB, cross(U,bW))) return false;

    if(Separated(cornersA, cornersB, cross(V,bU))) return false;
    if(Separated(cornersA, cornersB, cross(V,bV))) return false;
    if(Separated(cornersA, cornersB, cross(V,bW))) return false;

    if(Separated(cornersA, cornersB, cross(W,bU))) return false;
    if(Separated(cornersA, cornersB, cross(W,bV))) return false;
    if(Separated(cornersA, cornersB, cross(W,bW))) return false;
    
    return true;
}

bool OrientedBox::IntersectsOrientedBox(OrientedBox* other)
{
    vec3& bU = other->U;
    vec3& bV = other->V;
    vec3& bW = other->W;
    vector<vec3> cornersA = GetCorners();
    vector<vec3> cornersB = other->GetCorners();


    if(Separated(cornersA, cornersB, U)) return false;
    if(Separated(cornersA, cornersB, V)) return false;
    if(Separated(cornersA, cornersB, W)) return false;

    if(Separated(cornersA, cornersB, bU)) return false;
    if(Separated(cornersA, cornersB, bV)) return false;
    if(Separated(cornersA, cornersB, bW)) return false;

    if(Separated(cornersA, cornersB, cross(U,bU))) return false;
    if(Separated(cornersA, cornersB, cross(U,bV))) return false;
    if(Separated(cornersA, cornersB, cross(U,bW))) return false;

    if(Separated(cornersA, cornersB, cross(V,bU))) return false;
    if(Separated(cornersA, cornersB, cross(V,bV))) return false;
    if(Separated(cornersA, cornersB, cross(V,bW))) return false;

    if(Separated(cornersA, cornersB, cross(W,bU))) return false;
    if(Separated(cornersA, cornersB, cross(W,bV))) return false;
    if(Separated(cornersA, cornersB, cross(W,bW))) return false;
    
    return true;
}

bool OrientedBox::IntersectsSphere(Sphere* other)
{
    throw "NOT IMPLEMENTED!";
}

bool OrientedBox::ContainsPoint(const Point& p)
{
    throw "NOT IMPLEMENTED!";
}

Volume* OrientedBox::GetTransformed(glm::mat4 trs) 
{
    throw "NOT IMPLEMENTED!";
}

} // namespace Geometry