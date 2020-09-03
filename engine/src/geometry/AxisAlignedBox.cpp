#include "geometry/AxisAlignedBox.hpp"
#include "utilities/MathUtils.hpp"

using namespace glm;

namespace Engine::Geometry
{

AxisAlignedBox::AxisAlignedBox(glm::vec3 min, glm::vec3 max) : Min(min), Max(max)
{

}

AxisAlignedBox::AxisAlignedBox(const AxisAlignedBox &other) : Min(other.Min), Max(other.Max)
{

}

bool AxisAlignedBox::IntersectsAxisAlignedBox(AxisAlignedBox* other)
{
    if(Min[0] > other->Max[0] || other->Min[0] > Max[0]) return false;
    if(Min[1] > other->Max[1] || other->Min[1] > Max[1]) return false;
    if(Min[2] > other->Max[2] || other->Min[2] > Max[2]) return false;
    return true;
}

bool AxisAlignedBox::IntersectsSphere(Sphere* other)
{
    throw "Not implemented";
}

Volume* AxisAlignedBox::GetTransformed(glm::mat4 trs)
{
    vec3 min, max;
    vec3 v1, v2, v3, v4;
    vec3 v5, v6, v7, v8;
    min = Min;
    max = Max;
    v1 = min;
    v2 = vec3(max.x, min.y, min.z);
    v3 = vec3(max.x, max.y, min.z);
    v4 = vec3(min.x, max.y, min.z);
    v5 = vec3(min.x, min.y, max.z);
    v6 = vec3(max.x, min.y, max.z);
    v7 = vec3(max.x, max.y, max.z);
    v8 = vec3(min.x, max.y, max.z);
    std::vector<vec3> vecs = {v1, v2, v3, v4, v5, v6, v7, v8 };
    min = vec3(10000);
    max = vec3(-10000);
    for(vec3& v : vecs)
    {
        v = trs * vec4(v,1.0f);
        min = Utilities::Min(min, v);
        max = Utilities::Max(max, v);
    }
    return new AxisAlignedBox(min, max);
}

AxisAlignedBox* AxisAlignedBox::FromVertexSet(std::vector<Rendering::Vertex> &vertices)
{
    glm::vec3 min = glm::vec3(0.0f);
    glm::vec3 max = glm::vec3(0.0f);

    for(auto& v : vertices)
    {
        min = Utilities::Min(min, v.Position);
        max = Utilities::Max(max, v.Position);
    }
    return new AxisAlignedBox(min, max);
}

};
