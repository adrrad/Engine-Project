#pragma once
#include "rendering/Quaternion.hpp"
#include "Serialisable.hpp"


#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/quaternion.hpp>

#include <vector>


namespace Engine::Core
{
class GameObject;
class EngineCore;
class Transform : public Serialisable
{
friend class EngineCore;
private:
    Transform* m_parent;
    std::vector<Transform*> m_children;
    
    /**
     * @brief Transforms to local space of a parent transform.
     * 
     * @param parent The new parent transform.
     */
    void TransformToLocalSpace(Transform* parent);

    /**
     * @brief Transforms to global space.
     * 
     */
    void TransformToGlobalSpace();

    bool IsChildOf(Transform* other);

    void AddChild(Transform* child);

    void RemoveChild(Transform* child);

public:
    Engine::Core::GameObject* gameObject;
    glm::vec3 position;
    Quaternion rotation;
    glm::vec3 scale;

    Transform();

    void SetParent(Transform* parent);

    __forceinline Transform* GetParent();

    __forceinline std::vector<Transform*> GetChildren();

    __forceinline glm::mat4 GetTranslationMatrix();
    
    __forceinline glm::mat4 GetRotationMatrix();

    __forceinline glm::mat4 GetScaleMatrix();

    glm::mat4 GetModelMatrix(bool globalSpace = true);

    glm::mat4 GetViewMatrix();

    /**
     * @brief Calculates the transform's right vector in world space
     * 
     * @return glm::vec3 
     */
    inline glm::vec3 Right();

    inline glm::vec3 Up();

    inline glm::vec3 Forward();

    glm::vec3 GetGlobalPosition();

    Quaternion GetGlobalRotation();

    void SetGlobalPosition(const glm::vec3& globalPosition);

    void SetGlobalRotation(Quaternion& globalRotation);

    void LookAt(glm::vec3 at, glm::vec3 up);

    std::shared_ptr<Utilities::JSON::JSONValue> Serialise() override;

    void Deserialise(std::shared_ptr<Utilities::JSON::JSONValue> json) override;

};



Transform* Transform::GetParent()
{
    return m_parent;
}

std::vector<Transform*> Transform::GetChildren()
{
    return m_children;
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

glm::vec3 Transform::Right()
{
    return glm::rotate(rotation.q, glm::vec4(1,0,0,0));
}

glm::vec3 Transform::Up()
{
    return glm::rotate(rotation.q, glm::vec4(0,1,0,0));
}

glm::vec3 Transform::Forward()
{
    return glm::rotate(rotation.q, glm::vec4(0,0,1,0));
}

} // namespace Engine::Rendering

