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

namespace Rendering
{
    

Transform::Transform()
{
    position = glm::vec3(0,0,0);
    rotation = glm::vec3(0,0,0);
    scale = glm::vec3(1,1,1);
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
    return rotX * rotY * rotZ;
}

glm::mat4 Transform::GetScaleMatrix()
{
    return glm::scale(scale);
}

glm::mat4 Transform::GetModelMatrix()
{
    return GetTranslationMatrix() * GetRotationMatrix() * GetScaleMatrix();
}

glm::mat4 Transform::GetViewMatrix()
{
    glm::vec3 dir = GetDirection();
    return glm::lookAt(position, position-dir, glm::vec3(0,1,0));
}

glm::vec3 Transform::GetDirection()
{
    float rotX = glm::radians(rotation.x);
    float rotY = glm::radians(rotation.y);
    float z = cos(rotY)*cos(rotX);
    float x = sin(rotY)*cos(rotX);
    float y = -sin(rotX);
    return glm::normalize(glm::vec3(x,y,z));
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

