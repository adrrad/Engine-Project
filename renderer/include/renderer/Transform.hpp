#pragma once
#include <glm/glm.hpp>

namespace Rendering
{

class Transform
{
private:

public:
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;

    Transform();

    glm::mat4 GetTranslationMatrix();
    
    glm::mat4 GetRotationMatrix();

    glm::mat4 GetScaleMatrix();

    glm::mat4 GetModelMatrix();

    glm::mat4 GetViewMatrix();

    glm::vec3 GetDirection();

    void LookAt(glm::vec3 at, glm::vec3 up);

};

} // namespace Rendering

