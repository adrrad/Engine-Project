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
    return rotZ * rotY * rotX;
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

} // namespace Rendering

