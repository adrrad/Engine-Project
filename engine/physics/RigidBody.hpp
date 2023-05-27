#pragma once

#include "physics/PhysicsManager.hpp"

#include "gameplay/Transform.hpp"

#include <stdint.h>

namespace Physics
{
typedef uint32_t RBHandle;

class RigidBody
{
friend class PhysicsManager;
private:
    PhysicsManager* m_physicsManager;
    RBHandle m_handle;
    Gameplay::Transform* m_transform;
    Gameplay::Transform m_previousTransform;
    Physics::ColliderInfo m_colliderInfo;

    RigidBody(RBHandle handle, Gameplay::Transform& transform, Physics::ColliderInfo colliderInfo)
    {
        m_handle = handle;
        m_transform = &transform;
        m_previousTransform = transform;
        m_physicsManager = PhysicsManager::GetInstance();
        m_colliderInfo = colliderInfo;
    }   

    bool m_isKinematic = false;
    bool m_isStatic = false;
    bool m_debugDraw = false;

    glm::vec3 m_angularFactor;
    glm::vec3 m_linearFactor;

public:

    __forceinline void SetDrawDebugLines(bool enabled)
    {
        if(m_debugDraw == enabled) return;
        m_debugDraw = enabled;
        m_physicsManager->SetDebugDraw(this, enabled);
    }

    __forceinline void SetKinematic(bool enabled)
    {
        if(m_isKinematic == enabled) return;
        m_isKinematic = enabled;
        m_physicsManager->SetKinematic(this, enabled);

    }

    __forceinline bool IsKinematic()
    {
        return m_isKinematic;
    }

    __forceinline void SetStatic(bool enabled)
    {
        if(m_isStatic == enabled) return;
        m_isStatic = enabled;
        m_physicsManager->SetStatic(this, enabled);
    }

    __forceinline bool IsStatic()
    {
        return m_isStatic;
    }

    __forceinline void SetLinearVelocity(glm::vec3 vel)
    {
        m_physicsManager->SetLinearVelocity(this, vel);
    }

    __forceinline glm::vec3 GetLinearVelocity()
    {
        return m_physicsManager->GetLinearVelocity(this);
    }

    __forceinline void AddForce(glm::vec3 force)
    {
        m_physicsManager->AddForce(this, force);
    }

    __forceinline void AddTorque(glm::vec3 torque)
    {
        m_physicsManager->AddTorque(this, torque);
    }

    __forceinline void ClearForces()
    {
        m_physicsManager->ClearForces(this);
    }

    __forceinline void SetAngularConstraints(glm::vec3 fac)
    {
        m_physicsManager->SetAngularConstraints(this, fac);
    }

    __forceinline glm::bvec3 GetAngularConstraints()
    {
        return m_physicsManager->GetAngularConstraints(this);
    }

    __forceinline void SetLinearConstraints(glm::vec3 fac)
    {
        m_physicsManager->SetLinearConstraints(this, fac);
    }

    __forceinline glm::bvec3 GetLinearConstraints()
    {
        return m_physicsManager->GetLinearConstraints(this);
    }
    
    __forceinline void SetMass(float mass)
    {
        m_physicsManager->SetMass(this, mass);
    }

    __forceinline float GetMass()
    {
        return m_physicsManager->GetMass(this);
    }

    __forceinline Physics::ColliderInfo GetColliderInfo()
    {
        return m_colliderInfo;
    }
};

} // namespace Physics
