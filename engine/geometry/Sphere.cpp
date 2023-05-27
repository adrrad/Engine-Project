#include "geometry/Sphere.hpp"

#include "geometry/AxisAlignedBox.hpp"
#include "geometry/Point.hpp"

#include <iostream>

namespace Geometry
{

Sphere::Sphere(glm::vec3 center, float radius) : Center(center), Radius(radius)
{

}

bool Sphere::IntersectsAxisAlignedBox(AxisAlignedBox* other)
{
    float d = 0.0f;
    for(int i = 0; i < 3; i++)
    {
        float e = Center[i] - other->Min[i];
        if(e < 0)
        {
            if(e < -Radius) return false;
            d = d + e*e;
        }
        else if ((e = Center[i] - other->Max[i]) > 0)
        {
            if(e > Radius) return false;
            d = d + e*e;
        }
    }
    if(d > Radius*Radius) return false;
    return true;
}

bool Sphere::IntersectsOrientedBox(OrientedBox* other) 
{
    throw "NOT IMPLEMENTED";
}

bool Sphere::IntersectsSphere(Sphere* other)
{
    float dist = glm::distance(Center, other->Center);
    return dist <= (Radius + other->Radius);
}

bool Sphere::ContainsPoint(const Point& p)
{
    return glm::distance(Center, {p.x, p.y, p.z}) < Radius;
}

Volume* Sphere::GetTransformed(glm::mat4 trs)
{
    throw "Not implemented";
    //TODO: How to handle scale?
    glm::vec3 translation = trs[3];
    return new Sphere(Center, Radius);
}

};
