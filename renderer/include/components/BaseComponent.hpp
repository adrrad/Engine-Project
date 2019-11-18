#pragma once

namespace Rendering
{
class SceneObject;    
} // namespace Rendering


namespace Components
{

class BaseComponent
{
    Rendering::SceneObject* _sceneObject;
public:
    BaseComponent(Rendering::SceneObject* sceneObject)
    {
        _sceneObject = sceneObject;
    }

    virtual void Update(float deltaTime);
};

} // namespace Components


