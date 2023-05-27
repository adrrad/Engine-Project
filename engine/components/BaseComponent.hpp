#pragma once

#include "EngineTypedefs.hpp"
#include "Serialisable.hpp"

#include <string>

namespace Gameplay
{
class GameObject;    
}

namespace Gameplay
{
class ComponentManager;

class BaseComponent : public Serialisable
{
friend class ComponentManager;
friend class GameObject;
private:
    void SetGameObject(GameObject* gameObject);
protected:
    std::string Name;
    ComponentID ID;
    bool enabled;
    GameObject* gameObject;
public:
    BaseComponent() = default;
    ComponentID GetID() { return ID; }
    virtual void Start() = 0;
    virtual void Update(float deltaTime) = 0;
    virtual void DrawGUI() {};
    virtual bool Enabled() final;
    inline const std::string GetName() { return Name; }
    inline virtual void SetEnabled(bool enabled) final { this->enabled = enabled; }
    inline GameObject* GetGameObject() { return gameObject; }

    virtual std::shared_ptr<Utilities::JSON::JSONValue> Serialise() override;

    virtual void Deserialise(std::shared_ptr<Utilities::JSON::JSONValue> json) override;
};


} // namespace Gameplay


