#pragma once

#include "EngineTypedefs.hpp"
#include "utilities/serialisation/Serialisation.hpp"

#include <string>

namespace Engine::Core
{
class GameObject;    
}

namespace Engine::Components
{
class ComponentManager;

class BaseComponent : public Utilities::Serialisation::Serialisable
{
friend class ComponentManager;
friend class Engine::Core::GameObject;
private:
    SERIALISABLE(BaseComponent, bool, enabled);
    void SetGameObject(Engine::Core::GameObject* gameObject);
    ComponentID ID;
protected:
    Engine::Core::GameObject* gameObject;
public:
    const SERIALISABLE(BaseComponent, std::string, Name);
    BaseComponent() = default;
    BaseComponent(const std::string name);
    inline ComponentID GetID() { return ID; }
    inline virtual void SetEnabled(bool enabled) final { this->enabled = enabled; }
    inline virtual bool IsEnabled() final { return this->enabled; }
    virtual void Start() = 0;
    virtual void Update(float deltaTime) = 0;
    virtual void DrawGUI() {};
    virtual void DrawInspectorGUI() {};
    virtual std::string GetSerialised(int indent) = 0;
};

} // namespace Engine::Components


