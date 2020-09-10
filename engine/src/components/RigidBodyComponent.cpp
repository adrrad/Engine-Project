#include "components/RigidBodyComponent.hpp"
#include "core/GameObject.hpp"

#include <imgui.h>

#include <iostream>

using namespace std;

namespace Components
{
Engine::Physics::PhysicsManager* _physicsManager;

void RigidBodyComponent::Initialize(Engine::Physics::ColliderInfo& colInfo, float mass)
{
    if(_physicsManager == nullptr) _physicsManager = Engine::Physics::PhysicsManager::GetInstance();
    _rigidBody = _physicsManager->CreateRigidBody(gameObject->transform, { colInfo } , 1.0f, this);
}

void RigidBodyComponent::Update(float deltaTime)
{
    // while(!_contacts.empty())
    // {
    //     auto& c = _contacts.front();
    //     cout << "Contact with: " << c.other->gameObject->Name << endl;
    //     cout << "New : " << (c.NewCollision ? "Yes" : "No") << endl;
    //     cout << "Ended: " << (c.EndCollision ? "Yes" : "No") << endl;
    //     _contacts.pop();
    // }
}

void RigidBodyComponent::DrawInspectorGUI()
{
    bool draw = _debugDraw;
    ImGui::Checkbox("Draw Collider", &_debugDraw);
    if(_debugDraw != draw)
    {
        _rigidBody->SetDrawDebugLines(_debugDraw);
    } 
}

}