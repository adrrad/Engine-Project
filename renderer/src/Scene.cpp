#include "renderer/Scene.hpp"



namespace Rendering
{
    
void Scene::Update(float deltaTime)
{

}

std::vector<SceneObject>* Scene::GetSceneObjects()
{
    return &_sceneObjects;
}

} // namespace Rendering
