#pragma once

namespace Rendering
{
class SceneObject;    
} // namespace Rendering


namespace Components
{

class BaseComponent
{
protected:
    Rendering::SceneObject* sceneObject;
public:
    BaseComponent(Rendering::SceneObject* sceneObject);
    virtual void Update(float deltaTime) = 0;
};

} // namespace Components


