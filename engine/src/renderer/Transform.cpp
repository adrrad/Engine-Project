#include "renderer/Transform.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/gtc/quaternion.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#include <renderer/Quaternion.hpp>
namespace Rendering
{

void Transform::TransformToLocalSpace(Transform* parent)
{
    if(parent == this) return;
    auto trs = GetModelMatrix(true) * glm::inverse(parent->GetModelMatrix(true));
    glm::quat rotation;
    glm::vec3 skew;
    glm::vec4 perspective;
    glm::decompose(trs,  scale, rotation, position, skew, perspective);
    rotation = Quaternion({rotation.x, rotation.y, rotation.z, rotation.w}).ToEuler();
}

void Transform::TransformToGlobalSpace()
{
    glm::quat rotation;
    glm::vec3 skew;
    glm::vec4 perspective;
    glm::decompose(GetModelMatrix(true), scale, rotation, position, skew, perspective);
    rotation = Quaternion({rotation.x, rotation.y, rotation.z, rotation.w}).ToEuler();
}


Transform::Transform()
{
    _parent = nullptr;
    position = glm::vec3(0,0,0);
    rotation = glm::vec3(0,0,0);
    scale = glm::vec3(1,1,1);
}

void Transform::SetParent(Transform* parent)
{
    if(_parent != nullptr) TransformToGlobalSpace();
    TransformToLocalSpace(parent);
    _parent = parent;
}

Transform* Transform::GetParent()
{
    return _parent;
}

glm::mat4 Transform::GetTranslationMatrix()
{
    return glm::translate(position);
}

glm::mat4 Transform::GetRotationMatrix()
{
    glm::mat4 rotX = glm::eulerAngleX(glm::radians(rotation.x));
    glm::mat4 rotY = glm::eulerAngleY(glm::radians(rotation.y));
    glm::mat4 rotZ = glm::eulerAngleZ(glm::radians(rotation.z));
    return Quaternion::FromEuler(rotation).AsMatrix();
    // return rotX * rotY * rotZ;
}

glm::mat4 Transform::GetScaleMatrix()
{
    return glm::scale(scale);
}

glm::mat4 Transform::GetModelMatrix(bool globalSpace)
{
    auto trs = GetTranslationMatrix() * GetRotationMatrix() * GetScaleMatrix();
    if(globalSpace && _parent != nullptr && _parent != this) trs = _parent->GetModelMatrix(true) * trs;
    return trs;
}

glm::mat4 Transform::GetViewMatrix()
{
    glm::mat4 rotx = Quaternion::FromEuler({rotation.x, 0.0f, 0.0f}).AsMatrix();
    glm::mat4 roty = Quaternion::FromEuler({0.0f, rotation.y, 0.0f}).AsMatrix();
    glm::mat4 t = glm::translate(position);
    return glm::inverse(t*roty*rotx);//glm::lookAt(position, position-dir, glm::vec3(0,1,0));
}

glm::vec3 Transform::GetDirection()
{
    float rotX = glm::radians(rotation.x);
    float rotY = glm::radians(rotation.y);
    float z = cos(rotY)*cos(rotX);
    float x = sin(rotY)*cos(rotX);
    float y = -sin(rotX);
    return Quaternion::FromEuler(rotation)*glm::vec3(0,0,1);
    // return glm::normalize(glm::vec3(x,y,z));
}

void Transform::LookAt(glm::vec3 at, glm::vec3 up)
{
    auto lookAtMat = glm::lookAt(position, at, up);
    float rotXangle, rotYangle, rotZangle;
    // http://gamedev.stackexchange.com/a/112271
    rotXangle = atan2(-lookAtMat[2][1], lookAtMat[2][2]);
    float cosYangle = (float)sqrt(pow(lookAtMat[0][0], 2) + pow(lookAtMat[1][0], 2));
    rotYangle = atan2(lookAtMat[2][0], cosYangle);
    float sinXangle = sin(rotXangle);
    float cosXangle = cos(rotXangle);
    rotZangle = atan2(cosXangle * lookAtMat[0][1] + sinXangle * lookAtMat[0][2], cosXangle * lookAtMat[1][1] + sinXangle * lookAtMat[1][2]);
    rotation = -glm::degrees(glm::vec3{rotXangle, rotYangle, rotZangle});
}

} // namespace Rendering

