#include "core/SceneManager.hpp"


namespace Engine::Core
{

SceneManager* SceneManager::instance;

SceneManager::SceneManager()
{
    instance = this;
    m_activeScene = nullptr;
}

void SceneManager::LoadScene(std::string name)
{
    throw "Not implemented";
}

void SceneManager::UnloadScene(std::string name)
{
    throw "Not implemented";
}

Scene* SceneManager::GetActiveScene()
{
    return m_activeScene;
}

Scene* SceneManager::CreateNewScene(std::string name)
{
    Scene* scene = new Scene(name);
    m_scenes.push_back(scene);
    if(m_activeScene == nullptr) m_activeScene = scene;
    return scene;
}

SceneManager* SceneManager::GetInstance()
{
    return instance;
}

} // namespace Engine::Core
