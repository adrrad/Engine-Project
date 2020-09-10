#pragma once

#include "physics/PhysicsManager.hpp"

#include <renderer/Transform.hpp>

#include <stdint.h>

namespace Engine::Physics
{
typedef uint32_t RBHandle;

class RigidBody
{
friend class PhysicsManager;
private:
    PhysicsManager* _physicsManager;
    RBHandle _handle;
    Rendering::Transform* _transform;
    Rendering::Transform _previousTransform;

    RigidBody(RBHandle handle, Rendering::Transform& transform)
    {
        _handle = handle;
        _transform = &transform;
        _previousTransform = transform;
        _physicsManager = PhysicsManager::GetInstance();
    }   

    bool _isKinematic = false;
    bool _isStatic = false;
    bool _debugDraw = false;

    glm::vec3 _angularFactor;
    glm::vec3 _linearFactor;

public:

    __forceinline void SetDrawDebugLines(bool enabled)
    {
        if(_debugDraw == enabled) return;
        _debugDraw = enabled;
        _physicsManager->SetDebugDraw(this, enabled);
    }

    __forceinline void SetKinematic(bool enabled)
    {
        if(_isKinematic == enabled) return;
        _isKinematic = enabled;
        _physicsManager->SetKinematic(this, enabled);

    }

    __forceinline bool IsKinematic()
    {
        return _isKinematic;
    }

    __forceinline void SetStatic(bool enabled)
    {
        if(_isStatic == enabled) return;
        _isStatic = enabled;
        _physicsManager->SetStatic(this, enabled);
    }

    __forceinline bool IsStatic()
    {
        return _isStatic;
    }

    __forceinline void SetLinearVelocity(glm::vec3 vel)
    {
        _physicsManager->SetLinearVelocity(this, vel);
    }

    __forceinline void AddForce(glm::vec3 force, glm::vec3 relPos)
    {
        _physicsManager->AddForce(this, force, relPos);
    }

    __forceinline void AddTorque(glm::vec3 torque)
    {
        _physicsManager->AddTorque(this, torque);
    }

    __forceinline void ClearForces()
    {
        _physicsManager->ClearForces(this);
    }

    __forceinline void SetAngularFactor(glm::vec3 fac)
    {
        _physicsManager->SetAngularFactor(this, fac);
    }

    __forceinline glm::vec3 GetAngularFactor()
    {
        return _physicsManager->GetAngularFactor(this);
    }

    __forceinline void SetLinearFactor(glm::vec3 fac)
    {
        _physicsManager->SetLinearFactor(this, fac);
    }

    __forceinline glm::vec3 GetLinearFactor()
    {
        return _physicsManager->GetLinearFactor(this);
    }
    
    __forceinline void SetMass(float mass, glm::vec3 inertia)
    {
        _physicsManager->SetMass(this, mass, inertia);
    }
};

} // namespace Engine::Physics
