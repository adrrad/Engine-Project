#include "renderer/Quaternion.hpp"
#define _USE_MATH_DEFINES
#include <math.h>
using namespace glm;
#include <glm/gtx/quaternion.hpp>
Quaternion Quaternion::conjugate()
{
    return Quaternion({-q.x, -q.y, -q.z, q.w});
}

Quaternion Quaternion::FromEuler(glm::vec3 euler)
{
    //From Wikipedia
    euler.x = radians(euler.x);
    euler.y = radians(euler.y);
    euler.z = radians(euler.z);
    float cy = cos(euler.z * 0.5f);
    float sy = sin(euler.z * 0.5f);
    float cp = cos(euler.y * 0.5f);
    float sp = sin(euler.y * 0.5f);
    float cr = cos(euler.x * 0.5f);
    float sr = sin(euler.x * 0.5f);

    vec4 q;
    q.w = cy * cp * cr + sy * sp * sr;
    q.x = cy * cp * sr - sy * sp * cr;
    q.y = sy * cp * sr + cy * sp * cr;
    q.z = sy * cp * cr - cy * sp * sr;
    
    float magnitude = Quaternion(q).Magnitude();
    return q;
}

Quaternion Quaternion::Slerp(Quaternion& a, Quaternion& b, float t)
{
    float d = dot(a.q, b.q);
    if(d > 0.95)
    {
        vec4 nq = normalize(a.q + t*(b.q - a.q));
        return nq;
    }
    float theta = acos(dot(a.q, b.q));
    float wa = (sin(1.0f-t)*theta)/sin(theta);
    float wb = sin(t*theta)/sin(theta);
    return Quaternion(normalize(wa*a.q+wb*b.q)); 
}

Quaternion::Quaternion(vec4 q)
{
    this->q = q;
}

Quaternion Quaternion::Inverse()
{
    vec4 con = {-q.x, -q.y, -q.z, q.w};
    float len = length(con);
    con = con/(len*len);
    return Quaternion(con);
}

glm::mat4 Quaternion::AsMatrix()
{
    float x = q.x;
    float y = q.y;
    float z = q.z;
    float w = q.w;
    float x2 = x*x;
    float y2 = y*y;
    float z2 = z*z;

    return {{1.0f-2.0f*y2-2.0f*z2, 2.0f*x*y-2.0f*z*w, 2.0f*x*z+2.0f*y*w, 0.0f}, //1st column
            {2.0f*x*y+2.0f*z*w, 1.0f-2.0f*x2-2.0f*z2, 2.0f*y*z-2.0f*x*w, 0.0f}, //2nd column
            {2.0f*x*z-2.0f*y*w, 2.0f*y*z+2.0f*x*w, 1.0f-2.0f*x2-2.0f*y2, 0.0f}, //3rd column
            {0.0f, 0.0f, 0.0f, 1.0f}};                                          //4th column
}

float Quaternion::Magnitude()
{
    return length(q);
}

vec3 Quaternion::ToEuler()
{
    float x = atan2(2.0f*(q.w*q.x+q.y*q.z), 1.0f-2.0f*(q.x*q.x+q.y*q.y));
    float y = asin(2.0f*(q.w*q.y - q.z*q.x));
    float z = atan2(2.0f*(q.w*q.z+q.x*q.y), 1.0f-2.0f*(q.y*q.y+q.z*q.z));
    return {degrees(x), degrees(y), degrees(z)};
}

Quaternion Quaternion::operator*(Quaternion& other)
{
    vec4 p = other.q;
    vec3 qv = {q.x, q.y, q.z};
    vec3 pv = {p.x, p.y, p.z};
    float qs = q.w;
    float ps = p.w;
    vec3 rv = ps*qv + qs*pv + cross(pv,qv);
    float rs = ps*qs - dot(pv,qv);
    return Quaternion({rv.x, rv.y, rv.z, rs});
}

glm::vec3  Quaternion::operator*(glm::vec3& vc)
{
    Quaternion v = Quaternion({vc.x, vc.y, vc.z, 0.0f});
    vec4 r = (*this * v * conjugate()).q; //Normally use inverse but when quaternion is unit length, conjugate can be used instead
    return {r.x, r.y, r.z};
}
