#include "rendering/Transform.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/gtc/quaternion.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#include "rendering/Quaternion.hpp"
#include "core/GameObject.hpp"

#include "utilities/StdUtilities.hpp"

namespace Engine::Rendering
{

void Transform::TransformToLocalSpace(Transform* parent)
{
    if(parent == this) return;
    auto ptrs = glm::inverse(parent->GetModelMatrix(true));
    auto trs = GetModelMatrix(true);
    auto combined = ptrs * trs;
    glm::quat rotation;
    glm::vec3 skew;
    glm::vec4 perspective;
    glm::decompose(combined,  scale, rotation, position, skew, perspective);
    this->rotation = Quaternion(rotation);
}

void Transform::TransformToGlobalSpace()
{
    auto trs = GetModelMatrix(true);
    glm::quat rotation;
    glm::vec3 skew;
    glm::vec4 perspective;
    glm::decompose(trs, scale, rotation, position, skew, perspective);
    this->rotation = Quaternion(rotation);
}

bool Transform::IsChildOf(Transform* other)
{
    Transform* p = this;
    while(p = p->_parent) if(p == other) return true;
    return false;
}

void Transform::AddChild(Transform* child)
{
    int index = Engine::Utilities::IndexOf(_children, child);
    if(index == -1) _children.push_back(child);
}

void Transform::RemoveChild(Transform* child)
{
    int index = Engine::Utilities::IndexOf(_children, child);
    if(index >= 0) _children.erase(_children.begin()+index);
}

Transform::Transform()
{
    _parent = nullptr;
    position = glm::vec3(0,0,0);
    rotation = Quaternion(glm::quat(1, 0, 0, 0));
    scale = glm::vec3(1,1,1);
}

//FIXME: Something going wrong when changing parents. Global positions correctly remain the same, but rotations don't.
void Transform::SetParent(Transform* parent)
{
    //Do not do anything if trying to set a child as parent
    if(parent == _parent || parent != nullptr && parent->IsChildOf(this)) return;
    //Remove this transform from the current parent
    if(_parent != nullptr) _parent->RemoveChild(this);

    if(parent == nullptr)
    {
        //Calculate global space position
        TransformToGlobalSpace();
        return;
    } 
    //Otherwise transform to the new space
    TransformToLocalSpace(parent);
    parent->AddChild(this);
    _parent = parent;
}

Transform* Transform::GetParent()
{
    return _parent;
}

std::vector<Transform*> Transform::GetChildren()
{
    return _children;
}

glm::mat4 Transform::GetTranslationMatrix()
{
    return glm::translate(position);
}

glm::mat4 Transform::GetRotationMatrix()
{
    return rotation.ToMatrix();
}

glm::mat4 Transform::GetScaleMatrix()
{
    return glm::scale(scale);
}

glm::mat4 Transform::GetModelMatrix(bool globalSpace)
{
    auto trs = GetTranslationMatrix() * GetRotationMatrix() * GetScaleMatrix();
    if(globalSpace && _parent != nullptr && _parent != this) trs = _parent->GetModelMatrix(true) * trs;
    return trs;
}

glm::mat4 Transform::GetViewMatrix()
{
    //FIXME: Won't work correctly if the object is a child of another. Use the global transformation
    glm::vec3 euler = rotation.ToEuler();
    glm::mat4 rotx = Quaternion::FromEuler({euler.x, 0.0f, 0.0f}).ToMatrix();
    glm::mat4 roty = Quaternion::FromEuler({0.0f, euler.y, 0.0f}).ToMatrix();
    glm::mat4 t = glm::translate(position);
    return glm::inverse(t*rotation.ToMatrix());//glm::lookAt(position, position-dir, glm::vec3(0,1,0));
}

glm::vec3 Transform::GetDirection()
{
    return rotation*glm::vec3(0,0,1);
    // return glm::normalize(glm::vec3(x,y,z));
}

glm::vec3 Transform::GetGlobalPosition()
{
    return GetModelMatrix(true)[3];
}

Quaternion Transform::GetGlobalRotation()
{
    return glm::quat_cast(GetModelMatrix(true));
}

void Transform::SetGlobalPosition(const glm::vec3& globalPosition)
{
    if(_parent == nullptr)
    {
        position = globalPosition;
        return;
    }
    position = (glm::inverse(_parent->GetModelMatrix(true)) * glm::translate(globalPosition))[3];
}

void Transform::SetGlobalRotation(Quaternion& globalRotation)
{
    if(_parent == nullptr)
    {
        rotation = globalRotation;
        return;
    }
    rotation = glm::quat_cast(glm::inverse(_parent->GetModelMatrix(true)) * globalRotation.ToMatrix());
}

void Transform::LookAt(glm::vec3 at, glm::vec3 up)
{
    auto lookAtMat = glm::lookAt(position, at, up);
    float rotXangle, rotYangle, rotZangle;
    // http://gamedev.stackexchange.com/a/112271
    rotXangle = atan2(-lookAtMat[2][1], lookAtMat[2][2]);
    float cosYangle = (float)sqrt(pow(lookAtMat[0][0], 2) + pow(lookAtMat[1][0], 2));
    rotYangle = atan2(lookAtMat[2][0], cosYangle);
    float sinXangle = sin(rotXangle);
    float cosXangle = cos(rotXangle);
    rotZangle = atan2(cosXangle * lookAtMat[0][1] + sinXangle * lookAtMat[0][2], cosXangle * lookAtMat[1][1] + sinXangle * lookAtMat[1][2]);
    rotation = Quaternion::FromEuler(-glm::degrees(glm::vec3{rotXangle, rotYangle, rotZangle}));
}

} // namespace Engine::Rendering

