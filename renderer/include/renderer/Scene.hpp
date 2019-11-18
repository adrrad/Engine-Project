#pragma once
#include "renderer/SceneObject.hpp"

#include <vector>

namespace Rendering
{
    
class Scene
{
private:
    std::vector<SceneObject> _sceneObjects;

public:
    void AddSceneObject(SceneObject object);
    void Update(float deltaTime);

    std::vector<SceneObject>* GetSceneObjects();
};

} // namespace Rendering
