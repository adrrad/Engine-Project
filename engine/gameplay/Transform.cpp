#include "gameplay/Transform.hpp"

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
#include <glm/gtx/quaternion.hpp>

#include "rendering/Quaternion.hpp"
#include "gameplay/GameObject.hpp"
#include "gameplay/Scene.hpp"

#include "utilities/Utilities.hpp"

namespace Gameplay
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
    while(p = p->m_parent) if(p == other) return true;
    return false;
}

void Transform::AddChild(Transform* child)
{
    int index = Utilities::IndexOf(m_children, child);
    if(index == -1) m_children.push_back(child);
}

void Transform::RemoveChild(Transform* child)
{
    int index = Utilities::IndexOf(m_children, child);
    if(index >= 0) m_children.erase(m_children.begin()+index);
}

Transform::Transform()
{
    m_parent = nullptr;
    position = glm::vec3(0,0,0);
    rotation = Quaternion(glm::quat(1, 0, 0, 0));
    scale = glm::vec3(1,1,1);
}

//FIXME: Something going wrong when changing parents. Global positions correctly remain the same, but rotations don't.
void Transform::SetParent(Transform* parent)
{
    //Do not do anything if trying to set a child as parent
    if(parent == m_parent || parent != nullptr && parent->IsChildOf(this)) return;
    //Remove this transform from the current parent
    if(m_parent != nullptr) m_parent->RemoveChild(this);

    if(parent == nullptr)
    {
        //Calculate global space position
        TransformToGlobalSpace();
        return;
    } 
    //Otherwise transform to the new space
    TransformToLocalSpace(parent);
    parent->AddChild(this);
    m_parent = parent;
}

glm::mat4 Transform::GetModelMatrix(bool globalSpace)
{
    auto trs = GetTranslationMatrix() * GetRotationMatrix() * GetScaleMatrix();
    if(globalSpace && m_parent != nullptr && m_parent != this) trs = m_parent->GetModelMatrix(true) * trs;
    return trs;
}

glm::mat4 Transform::GetViewMatrix()
{
    return glm::lookAt(position, position - Forward(), {0,1,0});
    
    glm::mat4 t = glm::translate(position);
    return glm::inverse(t*rotation.ToMatrix());
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
    if(m_parent == nullptr)
    {
        position = globalPosition;
        return;
    }
    position = (glm::inverse(m_parent->GetModelMatrix(true)) * glm::translate(globalPosition))[3];
}

void Transform::SetGlobalRotation(Quaternion& globalRotation)
{
    if(m_parent == nullptr)
    {
        rotation = globalRotation;
        return;
    }
    rotation = glm::quat_cast(glm::inverse(m_parent->GetModelMatrix(true)) * globalRotation.ToMatrix());
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


std::shared_ptr<Utilities::JSON::JSONValue> Transform::Serialise()
{
    using namespace Utilities::JSON;
    glm::vec3 p = GetGlobalPosition();
    Quaternion q = GetGlobalRotation();
    glm::vec3& s = scale;

    auto serialiseChildrenIDs = [&]()
    {
        std::vector<std::shared_ptr<JSONValue>> childrenIDs;
        for(auto child : m_children)
        {
            childrenIDs.push_back(JSONValue::AsString(child->gameObject->GetID().ToString()));
        }
        return JSONValue::AsArray(childrenIDs);
    };
    GameObjectID parentID = m_parent == nullptr ? Engine::GUID() : m_parent->gameObject->GetID();
    return JSONValue::AsObject({
        {"position", JSONValue::AsArray({JSONValue::AsFloat(p.x), JSONValue::AsFloat(p.y), JSONValue::AsFloat(p.z)})},
        {"rotation", JSONValue::AsArray({JSONValue::AsFloat(q.w), JSONValue::AsFloat(q.x), JSONValue::AsFloat(q.y), JSONValue::AsNumber(q.z)})},
        {"scale", JSONValue::AsArray({JSONValue::AsFloat(s.x), JSONValue::AsFloat(s.y), JSONValue::AsFloat(s.z)})},
        {"parent", JSONValue::AsString(parentID.ToString())},
        {"children", serialiseChildrenIDs()}
    });
}


void Transform::Deserialise(std::shared_ptr<Utilities::JSON::JSONValue> json)
{
    // Parent (CURRENTLY UNNECESSARY)
    // Children
    auto childrenIDs = json->GetMember("children")->Array;
    for(auto child : childrenIDs)
    {
        Engine::GUID childID = child->String;
        GameObject* child = Scene::GetMainScene()->FindOrInstantiateGameObject(childID);
        child->transform.SetParent(this);
    }
    
    // Position, rotation, scale
    auto pos = json->GetMember("position");
    auto rot = json->GetMember("rotation");
    auto sca = json->GetMember("scale");
    position = { pos->Array[0]->Float, pos->Array[1]->Float, pos->Array[2]->Float };
    rotation = { rot->Array[0]->Float, rot->Array[1]->Float, rot->Array[2]->Float, rot->Array[3]->Float };
    scale = { sca->Array[0]->Float, sca->Array[1]->Float, sca->Array[2]->Float };
}

} // namespace Rendering

