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

class BaseComponent : public Utilities::Serialisation::Serialisable<BaseComponent>
{
friend class ComponentManager;
friend class Engine::Core::GameObject;
private:
    void SetGameObject(Engine::Core::GameObject* gameObject);
protected:
    SERIALISABLE(BaseComponent, std::string, Name);
    SERIALISABLE(BaseComponent, ComponentID, ID);
    SERIALISABLE(BaseComponent, bool, enabled);
    Engine::Core::GameObject* gameObject;
public:
    BaseComponent() = default;
    ComponentID GetID();// { return ID; }
    virtual void Start() = 0;
    virtual void Update(float deltaTime) = 0;
    virtual void DrawGUI() = 0;
    virtual void DrawInspectorGUI() = 0;
    virtual bool Enabled() final;
    inline const std::string GetName() { return Name; }
    inline virtual void SetEnabled(bool enabled) final { this->enabled = enabled; }
    inline Core::GameObject* GetGameObject() { return gameObject; }
};

template <class C>
class Component : public BaseComponent
{
friend class ComponentManager;
friend class Engine::Core::GameObject;
private:
protected:
public:
    Component() { SerialiseBaseClass<BaseComponent>(); };
    Component(const std::string name) 
    { 
        Utilities::Serialisation::SerialiseProperty<C>(offsetof(Component, enabled), "enabled", enabled);
        Utilities::Serialisation::SerialiseProperty<C>(offsetof(Component, ID), "ID", ID);
        Utilities::Serialisation::SerialiseProperty<C>(offsetof(Component, Name), "Name", Name);
        Name = name; 
    }
    inline ComponentID GetID() { return ID; }


    virtual void Start() = 0;
    virtual void Update(float deltaTime) = 0;
    virtual void DrawGUI() {};
    virtual void DrawInspectorGUI() {};
    inline Utilities::JSON::JSONValue* GetSerialised() override;
};

template <class C>
Utilities::JSON::JSONValue* Component<C>::GetSerialised()
{
    return Utilities::Serialisation::SerialiseObject(dynamic_cast<C*>(this));
}

} // namespace Engine::Components


