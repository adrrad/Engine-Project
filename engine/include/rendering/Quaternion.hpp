#pragma once

#include<glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

// yaw (Z), pitch (Y), roll (X)
class Quaternion
{

private:

    __forceinline Quaternion conjugate();

public:
    union
    {
        glm::quat q;
        struct
        {
            float w, x, y, z;
        };
    };

    __forceinline static Quaternion FromEuler(glm::vec3 euler);

    __forceinline static Quaternion Slerp(Quaternion& a, Quaternion& b, float t);

    __forceinline Quaternion();

    __forceinline Quaternion(glm::quat q);

    __forceinline Quaternion(float x, float y, float z, float w);

    __forceinline Quaternion Inverse();

    __forceinline glm::mat4 ToMatrix();

    __forceinline float Magnitude();

    __forceinline glm::vec3 ToEuler();

    // __forceinline Quaternion operator*(Quaternion& other);

    // __forceinline glm::vec3 operator*(glm::vec3& v);

    __forceinline Quaternion operator*(Quaternion other);

    __forceinline glm::vec3 operator*(glm::vec3 v);

};

Quaternion Quaternion::conjugate()
{
    return glm::conjugate(q);
}

Quaternion Quaternion::FromEuler(glm::vec3 euler)
{
    return glm::quat(glm::radians(euler));
}

Quaternion Quaternion::Slerp(Quaternion& a, Quaternion& b, float t)
{
    auto q = glm::mix(a.q, b.q, t);
    return q; 
}

Quaternion::Quaternion()
{
    this->q = glm::quat(1, 0, 0, 0);
}

Quaternion::Quaternion(glm::quat q)
{
    this->q = q;
}

Quaternion::Quaternion(float x, float y, float z, float w)
{
    this->q = glm::quat(w, x, y, z);
}

Quaternion Quaternion::Inverse()
{
    return glm::inverse(q);
}

glm::mat4 Quaternion::ToMatrix()
{
    return glm::mat4_cast(q);
}

float Quaternion::Magnitude()
{
    return length(q);
}

glm::vec3 Quaternion::ToEuler()
{
    
    return glm::eulerAngles(q) * 180.0f / glm::pi<float>();
}

// Quaternion Quaternion::operator*(Quaternion& other)
// {

//     return q * other.q;
// }

// glm::vec3  Quaternion::operator*(glm::vec3& vc)
// {
//     return q * vc;
// }

Quaternion Quaternion::operator*(Quaternion other)
{

    return q * other.q;
}

glm::vec3  Quaternion::operator*(glm::vec3 vc)
{
    return q * vc;
}