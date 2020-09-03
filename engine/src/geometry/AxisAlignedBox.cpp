#include "geometry/AxisAlignedBox.hpp"


namespace Engine::Geometry
{

AxisAlignedBox::AxisAlignedBox(glm::vec3 min, glm::vec3 max) : Min(min), Max(max)
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
    glm::vec4 min, max;
    glm::vec3 newMin, newMax;
    min = trs * glm::vec4(Min, 1.0f);
    max = trs * glm::vec4(Max, 1.0f);
    newMin.x = min.x < max.x ? min.x : max.x;
    newMin.y = min.y < max.y ? min.y : max.y;
    newMin.z = min.x < max.z ? min.z : max.z;
    newMax.x = max.x > min.x ? max.x : min.x;
    newMax.y = max.y > min.y ? max.y : min.y;
    newMax.z = max.x > min.z ? max.z : min.z;
    return new AxisAlignedBox(newMin, newMax);
}

AxisAlignedBox* AxisAlignedBox::FromVertexSet(std::vector<Rendering::Vertex> &vertices)
{
    glm::vec3 min = glm::vec3(0.0f);
    glm::vec3 max = glm::vec3(0.0f);

    for(auto& v : vertices)
    {
        min.x = min.x < v.Position.x ? min.x : v.Position.x;
        min.y = min.y < v.Position.y ? min.y : v.Position.y;
        min.z = min.x < v.Position.z ? min.z : v.Position.z;
        max.x = max.x > v.Position.x ? max.x : v.Position.x;
        max.y = max.y > v.Position.y ? max.y : v.Position.y;
        max.z = max.x > v.Position.z ? max.z : v.Position.z;
    }
    return new AxisAlignedBox(min, max);
}

};
