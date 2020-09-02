#pragma once

namespace Rendering
{
class GameObject;    
} // namespace Rendering


namespace Components
{

class BaseComponent
{
friend class Rendering::GameObject;
private:
    void SetGameObject(Rendering::GameObject* gameObject);
protected:
    Rendering::GameObject* gameObject;
public:
    BaseComponent() = default;
    BaseComponent(Rendering::GameObject* gameObject);
    virtual void Update(float deltaTime) = 0;
    virtual void DrawGUI() {};
};

} // namespace Components


