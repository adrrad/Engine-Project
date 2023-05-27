#pragma once

#include <glm/glm.hpp>

namespace Gameplay
{
    class RigidBodyComponent;
} // namespace Gameplay


namespace Physics
{

struct Colliders
{
    Gameplay::RigidBodyComponent* A;
    Gameplay::RigidBodyComponent* B;
};

struct ContactInfo
{
    Gameplay::RigidBodyComponent* other;
    glm::vec3 Point;
    glm::vec3 Normal;
    bool NewCollision;
    bool EndCollision;
};



} // namespace Physics
