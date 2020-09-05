#pragma once

#include <string>

namespace Engine::Core
{
class GameObject;    
}

namespace Components
{

class BaseComponent
{
friend class Engine::Core::GameObject;
private:
    void SetGameObject(Engine::Core::GameObject* gameObject);
protected:
    Engine::Core::GameObject* gameObject;
public:
    const std::string Name;
    BaseComponent() = default;
    BaseComponent(const std::string name);
    virtual void Update(float deltaTime) = 0;
    virtual void DrawGUI() {};
    virtual void DrawInspectorGUI() {};

};

} // namespace Components


