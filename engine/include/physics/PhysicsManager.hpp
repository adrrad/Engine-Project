#pragma once

#include "renderer/Transform.hpp"

#include <glm/glm.hpp>

#include <vector>

namespace Engine::Physics
{
class RigidBody;

enum class ColliderType
{
    BOX,
    CYLINDER,
    CAPSULE,
    PLANE,
    SPHERE
};

struct ColliderInfo 
{
    Rendering::Transform Transform;
    ColliderType Type;
    union
    {
        struct
        {
            glm::vec3 HalfExtents;
        } Box, Cyllinder;

        struct
        {
            float Radius;
        } Sphere;

        struct
        {
            glm::vec3 N;
            float D;
        } Plane;
        struct
        {
            float Radius;
            float Height;
        } Capsule;
    };
};

class PhysicsManager
{
    static PhysicsManager* _instance;
    PhysicsManager();

    bool _debugDrawEnabled = false;

public:

    ~PhysicsManager();

    void SetDebugDraw(bool enabled);

    static PhysicsManager* GetInstance();

    void Step(float deltaTime);

    void Update(float deltaTime);

    RigidBody* CreateRigidBody(Rendering::Transform &transform, std::vector<ColliderInfo> colliders, float mass, void* owner);

    void RemoveRigidBody(RigidBody* rb);

    glm::vec3 GetGravity();

    void SetGravity(glm::vec3 gravity);

    void SynchonizeTransforms();

    void SetLinearVelocity(RigidBody* rb, glm::vec3 vel);

    void AddForce(RigidBody* rb, glm::vec3 force, glm::vec3 relPos);

    void AddTorque(RigidBody* rb, glm::vec3 torque);

    void ClearForces(RigidBody* rb);

    void SetAngularFactor(RigidBody* rb, glm::vec3 fac);

    glm::vec3 GetAngularFactor(RigidBody* rb);

    void SetLinearFactor(RigidBody* rb, glm::vec3 fac);

    glm::vec3 GetLinearFactor(RigidBody* rb);
    
    void SetMass(RigidBody* rb, float mass, glm::vec3 inertia);
};

} // namespace Engine::Physics
