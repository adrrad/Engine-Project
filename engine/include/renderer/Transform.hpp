#pragma once

#include <glm/glm.hpp>
#include <vector>

namespace Engine::Core
{
    class GameObject;
}

namespace Rendering
{

class Transform
{

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
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;

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

    void LookAt(glm::vec3 at, glm::vec3 up);

};

} // namespace Rendering

