#pragma once

#include <string>

namespace Engine::Core
{
class GameObject;    
}

namespace Engine::Components
{

class BaseComponent
{
friend class Engine::Core::GameObject;
private:
    bool enabled;
    void SetGameObject(Engine::Core::GameObject* gameObject);
protected:
    Engine::Core::GameObject* gameObject;
public:
    const std::string Name;
    BaseComponent() = default;
    BaseComponent(const std::string name);
    inline virtual void SetEnabled(bool enabled) final { this->enabled = enabled; }
    inline virtual bool IsEnabled() final { return this->enabled; }
    virtual void Start() = 0;
    virtual void Update(float deltaTime) = 0;
    virtual void DrawGUI() {};
    virtual void DrawInspectorGUI() {};

};

} // namespace Engine::Components


