#include "physics/RigidBody.hpp"



namespace Engine::Physics
{

RigidBody::RigidBody(RBHandle handle, Rendering::Transform& transform)
{
    _handle = handle;
    _transform = &transform;
    _previousTransform = transform;
}    

void RigidBody::SetKinematic(bool isKinematic)
{
    _isKinematic = isKinematic;
}

bool RigidBody::IsKinematic()
{
    return _isKinematic;
}

void RigidBody::SetStatic(bool isStatic)
{
    _isStatic = isStatic;
}

bool RigidBody::IsStatic()
{
    return _isStatic;
}


} // namespace Engine::Physics
