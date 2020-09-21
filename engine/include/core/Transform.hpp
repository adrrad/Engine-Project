#pragma once
#include "rendering/Quaternion.hpp"
#include "utilities/serialisation/Serialisation.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <vector>


namespace Engine::Core
{
class GameObject;
class EngineCore;
class Transform : public Utilities::Serialisation::Serialisable
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

    inline std::string GetSerialised(int indent) override;

    Transform();

    void SetParent(Transform* parent);

    Transform* GetParent();

    std::vector<Transform*> GetChildren();

    glm::mat4 GetTranslationMatrix();
    
    glm::mat4 GetRotationMatrix();

    glm::mat4 GetScaleMatrix();

    glm::mat4 GetModelMatrix(bool globalSpace = true);

    glm::mat4 GetViewMatrix();

    glm::vec3 GetDirection();

    glm::vec3 GetGlobalPosition();

    Quaternion GetGlobalRotation();

    void SetGlobalPosition(const glm::vec3& globalPosition);

    void SetGlobalRotation(Quaternion& globalRotation);

    void LookAt(glm::vec3 at, glm::vec3 up);

};

std::string Transform::GetSerialised(int indent)
{
    return Utilities::Serialisation::SerializeObject(this, indent);
}

} // namespace Engine::Rendering

