#pragma once

namespace Engine::Core
{
class GameObject;    
} // namespace Rendering


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
    BaseComponent() = default;
    BaseComponent(Engine::Core::GameObject* gameObject);
    virtual void Update(float deltaTime) = 0;
    virtual void DrawGUI() {};
};

} // namespace Components


