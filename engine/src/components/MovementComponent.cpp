#include "components/MovementComponent.hpp"
#include "renderer/WindowManager.hpp"
#include "core/GameObject.hpp"

#include <iostream>
#include <algorithm>
#include <math.h>

using namespace glm;
using namespace std;


namespace Components
{
using namespace Rendering;
MovementComponent::MovementComponent()
{
    auto winMan = WindowManager::GetInstance();
    winMan->RegisterMousePositionCallback([&](double dx, double dy)
    {
        if(_mouseLocked)
        {
            this->gameObject->transform.rotation.y -= float(dx)*_rotationSpeed;
            float& x = this->gameObject->transform.rotation.x;
            x = std::min(std::max(x - float(dy)*_rotationSpeed, -85.0f), 85.0f);
        }
    });

    winMan->RegisterKeyCallback([&](int key, int action)
    {
        auto winMan = WindowManager::GetInstance();
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
            case GLFW_KEY_TAB:
            if(action == GLFW_PRESS)
            {
                _mouseLocked = !_mouseLocked;
                if(_mouseLocked) winMan->LockCursor(winMan->GetActiveWindow());
                else winMan->UnlockCursor(winMan->GetActiveWindow());
            }
            default:
            break;
        }
    });
}
    
void MovementComponent::Update(float deltaTime)
{
        Transform* transform = &this->gameObject->transform;
        glm::mat4 rotx = Quaternion::FromEuler({transform->rotation.x, 0.0f, 0.0f}).AsMatrix();
        glm::mat4 roty = Quaternion::FromEuler({0.0f, transform->rotation.y, 0.0f}).AsMatrix();
        vec3 dir = (roty*rotx)*vec4(0,0,-1,0);
        // dir = rot*vec3(0,0,-1);
        vec3 forward = dir * _movementSpeed * deltaTime; //Negative since camera
        vec3 side = -normalize(cross(forward, vec3(0,1,0))) * _movementSpeed * deltaTime;
        vec3 up = vec3(0, 1, 0) * _movementSpeed *deltaTime;
        if(this->forward) transform->position += forward;
        if(this->left) transform->position += side;
        if(this->backward) transform->position -= forward;
        if(this->right) transform->position -= side;
        if(this->up) transform->position += up;
        if(this->down) transform->position -= up;
}

void MovementComponent::SetCamera(CameraComponent* camera)
{
    this->camera = camera;
}

void MovementComponent::SetWaveManager(WaveManagerComponent* waveManager)
{
    this->waveManager = waveManager;
}


} // namespace Components