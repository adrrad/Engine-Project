#pragma once

#include <renderer/Transform.hpp>

#include <stdint.h>

namespace Engine::Physics
{

class PhysicsManager;
typedef uint32_t RBHandle;

class RigidBody
{
friend class PhysicsManager;
private:
    RBHandle _handle;
    Rendering::Transform* _transform;
    Rendering::Transform _previousTransform;

    RigidBody(RBHandle handle, Rendering::Transform& transform);

    bool _isKinematic = false;
    bool _isStatic = false;

public:

    // void SetLinearVelocity(glm::vec3 vel);

    // void AddForce(glm::vec3 force, glm::vec3 relPos);

    // void AddTorque(glm::vec3 torque);

    // void ClearForces();

    void SetKinematic(bool isKinematic);

    bool IsKinematic();

    void SetStatic(bool isStatic);

    bool IsStatic();

    void SetAngularFactor(glm::vec3 fac);

    glm::vec3 GetAngularFactor();

    void SetLinearFactor(glm::vec3 fac);

    glm::vec3 GetLinearFactor();
    
    void SetMass(float mass, glm::vec3 inertia);

};

} // namespace Engine::Physics
