#include "components/MovementComponent.hpp"
#include "renderer/WindowManager.hpp"
#include "renderer/SceneObject.hpp"

#include <iostream>
#include <algorithm>
#include <math.h>

using namespace glm;
using namespace std;


namespace Components
{
using namespace Rendering;
MovementComponent::MovementComponent(SceneObject* sceneObject)
: BaseComponent(sceneObject)
{
    auto winMan = WindowManager::GetInstance();

    winMan->RegisterMousePositionCallback([&](double dx, double dy)
    {
        this->sceneObject->transform.rotation.y += float(dx)*_rotationSpeed;
        float& x = this->sceneObject->transform.rotation.x;
        x = std::min(std::max(x + float(-dy)*_rotationSpeed, -85.0f), 85.0f);
    });

    winMan->RegisterKeyCallback([&](int key, int action)
    {

        switch(key)
        {
            case GLFW_KEY_W:
            forward = action == GLFW_PRESS || action == GLFW_REPEAT;
            break;
            case GLFW_KEY_A:
            left = action == GLFW_PRESS || action == GLFW_REPEAT;
            break;
            case GLFW_KEY_S:
            backward = action == GLFW_PRESS || action == GLFW_REPEAT;
            break;
            case GLFW_KEY_D:
            right = action == GLFW_PRESS || action == GLFW_REPEAT;
            break;
            case GLFW_KEY_SPACE:
            up = action == GLFW_PRESS || action == GLFW_REPEAT;
            break;
            case GLFW_KEY_LEFT_SHIFT:
            down = action == GLFW_PRESS || action == GLFW_REPEAT;
            break;
            default:
            break;
        }
    });
}
    
void MovementComponent::Update(float deltaTime)
{
        Transform* transform = &this->sceneObject->transform;
        vec3 forward = -transform->GetDirection() * _movementSpeed * deltaTime; //Negative since camera
        vec3 side = -normalize(cross(forward, vec3(0,1,0))) * _movementSpeed * deltaTime;
        vec3 up = vec3(0, 1, 0)*deltaTime;
        if(this->forward) transform->position += forward;
        if(this->left) transform->position += side;
        if(this->backward) transform->position -= forward;
        if(this->right) transform->position -= side;
        if(this->up) transform->position += up;
        if(this->down) transform->position -= up;
}

} // namespace Components
