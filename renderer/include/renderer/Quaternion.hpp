#pragma once

#include<glm/glm.hpp>

// yaw (Z), pitch (Y), roll (X)
class Quaternion
{

private:
    glm::vec4 q;

    Quaternion conjugate();

public:

    static Quaternion FromEuler(glm::vec3 euler);

    static Quaternion Slerp(Quaternion& a, Quaternion& b, float t);

    Quaternion(glm::vec4 q);

    Quaternion Inverse();

    glm::mat4 AsMatrix();

    float Magnitude();

    glm::vec3 ToEuler();

    Quaternion operator*(Quaternion& other);

    glm::vec3 operator*(glm::vec3& v);

};