#pragma once
#include "rendering/Quaternion.hpp"
#include "utilities/serialisation/Serialisation.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#include <vector>


namespace Engine::Core
{
class GameObject;
class EngineCore;
class Transform : public Utilities::Serialisation::Serialisable<Transform>
{
friend class EngineCore;
private:
    Transform* _parent;
    std::vector<Transform*> _children;
    
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
    SERIALISABLE(Transform, glm::vec3, position);
    SERIALISABLE(Transform, Quaternion, rotation);
    SERIALISABLE(Transform, glm::vec3, scale);

    Transform();

    void SetParent(Transform* parent);

    __forceinline Transform* GetParent();

    __forceinline std::vector<Transform*> GetChildren();

    __forceinline glm::mat4 GetTranslationMatrix();
    
    __forceinline glm::mat4 GetRotationMatrix();

    __forceinline glm::mat4 GetScaleMatrix();

    glm::mat4 GetModelMatrix(bool globalSpace = true);

    glm::mat4 GetViewMatrix();

    glm::vec3 GetDirection();

    glm::vec3 GetGlobalPosition();

    Quaternion GetGlobalRotation();

    void SetGlobalPosition(const glm::vec3& globalPosition);

    void SetGlobalRotation(Quaternion& globalRotation);

    void LookAt(glm::vec3 at, glm::vec3 up);

};



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

} // namespace Engine::Rendering

