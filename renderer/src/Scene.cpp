#include "renderer/Scene.hpp"



namespace Rendering
{


void Scene::AddSceneObject(SceneObject *object)
{
    _sceneObjects.push_back(object);
}
    
void Scene::Update(float deltaTime)
{
    for(auto obj : _sceneObjects)
    {
        obj->Update(deltaTime);
    }
}

std::vector<SceneObject*>* Scene::GetSceneObjects()
{
    return &_sceneObjects;
}

} // namespace Rendering
