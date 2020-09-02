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
    void SetGameObject(Rendering::GameObject* GameObject);
protected:
    Rendering::GameObject* GameObject;
public:
    BaseComponent() = default;
    BaseComponent(Rendering::GameObject* GameObject);
    virtual void Update(float deltaTime) = 0;
    virtual void DrawGUI() {};
};

} // namespace Components


