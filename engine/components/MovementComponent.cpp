#include "components/MovementComponent.hpp"
#include "platform/WindowManager.hpp"
#include "gameplay/GameObject.hpp"
#include "rendering/Quaternion.hpp"
#include "components/RigidBodyComponent.hpp"
#include "components/CameraComponent.hpp"

#include <imgui.h>
#include <glm/gtx/rotate_vector.hpp>

#include <iostream>
#include <algorithm>
#include <math.h>
#include "MovementComponent.hpp"


using namespace glm;
using namespace std;

static bool spacePressed = false;
namespace Gameplay
{
using namespace Rendering;
MovementComponent::MovementComponent()
{

}

void MovementComponent::Start()
{
    auto winMan = Platform::WindowManager::GetInstance();
    winMan->RegisterMousePositionCallback([&](double dx, double dy)
    {
        if(m_mouseLocked && Enabled())
        {
            eulerOffset.y += float(dx)*m_rotationSpeed;
            float x = eulerOffset.x;
            eulerOffset.x = std::min(std::max(x - float(-dy)*m_rotationSpeed, -85.0f), 85.0f);
            glm::vec3 euler = this->gameObject->transform.rotation.ToEuler();
            this->gameObject->transform.rotation = Quaternion::FromEuler(eulerOffset);
        }
    });

    winMan->RegisterKeyCallback([&](int key, int action)
    {
        auto winMan = Platform::WindowManager::GetInstance();
        
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
            {
                this->up = (action == GLFW_PRESS) && (action != GLFW_RELEASE);
                
            }
            break;
            case GLFW_KEY_LEFT_SHIFT:
            down = action == GLFW_PRESS || action == GLFW_REPEAT;
            break;
            case GLFW_KEY_TAB:
            if(action == GLFW_PRESS)
            {
                m_mouseLocked = !m_mouseLocked;
                if(m_mouseLocked) winMan->LockCursor(winMan->GetActiveWindow());
                else winMan->UnlockCursor(winMan->GetActiveWindow());
            }
            default:
            break;
        }
    });
    
}

void MovementComponent::Update(float deltaTime)
{
    gameObject->GetComponent<CameraComponent>()->SetMain();
    if(rigidbodycomp == nullptr) rigidbodycomp = gameObject->GetComponent<RigidBodyComponent>();
    Gameplay::Transform* transform = &this->gameObject->transform;
    bool jump = this->up && !spacePressed;
    spacePressed = this->up;
    vec3 dir = transform->rotation*vec3(0,0,-1);
    // dir = rot*vec3(0,0,-1);
    vec3 forward = dir; //Negative since camera
    forward.y = 0;
    forward = normalize(forward) * m_movementSpeed * deltaTime;
    vec3 side = -normalize(cross(forward, vec3(0,1,0))) * m_movementSpeed * deltaTime;
    vec3 up = vec3(0, 1, 0) * m_movementSpeed *deltaTime;
    if(this->forward) transform->position += forward;
    if(this->left) transform->position += side;
    if(this->backward) transform->position -= forward;
    if(this->right) transform->position -= side;
    if(this->down) transform->position -= up;

    if(rigidbodycomp == nullptr)
    {
        return;
    }

    if(jump && abs(rigidbodycomp->GetRigidBody().GetLinearVelocity().y) < 0.1f)
    {
        rigidbodycomp->GetRigidBody().AddForce({0, 300, 0});
    } 
}

void MovementComponent::SetCamera(CameraComponent* camera)
{
    this->camera = camera;
}

void MovementComponent::SetWaveManager(WaveManagerComponent* waveManager)
{
    this->waveManager = waveManager;
}

void MovementComponent::DrawGUI()
{
    ImGui::Begin("Window");
    ImGui::DragFloat3("Rotation:", &gameObject->transform.rotation.ToEuler()[0]);
    ImGui::End();
}

} // namespace Gameplay
