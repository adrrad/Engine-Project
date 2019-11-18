#include "renderer/Scene.hpp"



namespace Rendering
{


void Scene::AddSceneObject(SceneObject *object)
{
    _sceneObjects.push_back(object);
}
    
void Scene::Update(float deltaTime)
{

}

std::vector<SceneObject*>* Scene::GetSceneObjects()
{
    return &_sceneObjects;
}

} // namespace Rendering
