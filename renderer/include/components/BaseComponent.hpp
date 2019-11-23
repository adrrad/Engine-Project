#pragma once

namespace Rendering
{
class SceneObject;    
} // namespace Rendering


namespace Components
{

class BaseComponent
{
friend class Rendering::SceneObject;
private:
    void SetSceneObject(Rendering::SceneObject* sceneObject);
protected:
    Rendering::SceneObject* sceneObject;
public:
    BaseComponent() = default;
    BaseComponent(Rendering::SceneObject* sceneObject);
    virtual void Update(float deltaTime) = 0;
    virtual void DrawGUI() {};
};

} // namespace Components


