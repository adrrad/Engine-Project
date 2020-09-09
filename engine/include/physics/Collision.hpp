#pragma once

#include <glm/glm.hpp>

namespace Components
{
    class RigidBodyComponent;
} // namespace Components


namespace Engine::Physics
{

struct Colliders
{
    Components::RigidBodyComponent* A;
    Components::RigidBodyComponent* B;
};

struct ContactInfo
{
    Components::RigidBodyComponent* other;
    glm::vec3 Point;
    glm::vec3 Normal;
    bool NewCollision;
    bool EndCollision;
};



} // namespace Engine::Physics
