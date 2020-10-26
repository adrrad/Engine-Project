#pragma once

#include "EngineTypedefs.hpp"

#include <string>

namespace Engine::Core
{
class GameObject;    
}

namespace Engine::Components
{
class ComponentManager;

class BaseComponent
{
friend class ComponentManager;
friend class Engine::Core::GameObject;
private:
    void SetGameObject(Engine::Core::GameObject* gameObject);
protected:
    std::string Name;
    ComponentID ID;
    bool enabled;
    Engine::Core::GameObject* gameObject;
public:
    BaseComponent() = default;
    ComponentID GetID() { return ID; }
    virtual void Start() = 0;
    virtual void Update(float deltaTime) = 0;
    virtual void DrawGUI() {};
    virtual void DrawInspectorGUI() {};
    virtual bool Enabled() final;
    inline const std::string GetName() { return Name; }
    inline virtual void SetEnabled(bool enabled) final { this->enabled = enabled; }
    inline Core::GameObject* GetGameObject() { return gameObject; }
};


} // namespace Engine::Components


