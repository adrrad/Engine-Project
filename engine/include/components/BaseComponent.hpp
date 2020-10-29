#pragma once

#include "EngineTypedefs.hpp"
#include "Serialisable.hpp"

#include <string>

namespace Engine::Core
{
class GameObject;    
}

namespace Engine::Components
{
class ComponentManager;

class BaseComponent : public Serialisable
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

    virtual std::shared_ptr<Utilities::JSON::JSONValue> Serialise() override;

    virtual void Deserialise(std::shared_ptr<Utilities::JSON::JSONValue> json) override;
};


} // namespace Engine::Components


