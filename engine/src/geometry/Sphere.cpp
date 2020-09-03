#include "geometry/Sphere.hpp"

#include <iostream>

namespace Engine::Geometry
{

Sphere::Sphere(glm::vec3 center, float radius) : Center(center), Radius(radius)
{

}

bool Sphere::IntersectsAxisAlignedBox(AxisAlignedBox* other)
{
    throw "Not implemented";
}

bool Sphere::IntersectsSphere(Sphere* other)
{
    float dist = glm::distance(Center, other->Center);
    return dist <= (Radius + other->Radius);
}

Volume* Sphere::GetTransformed(glm::mat4 trs)
{
    throw "Not implemented";
    //TODO: How to handle scale?
    glm::vec3 translation = trs[3];
    return new Sphere(Center, Radius);
}

};
