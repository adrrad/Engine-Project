#pragma once

#include "Scene.hpp"

// #include "assets/AssetManager.hpp"


#include <string>
#include <vector>


namespace Engine::Core
{
    
class SceneManager
{

private:
    static SceneManager* instance;

    std::vector<Scene*> m_scenes;
    Scene* m_activeScene;

public:

    /**
     * @brief Construct a new Scene Manager object
     * 
     */
    SceneManager();

    /**
     * @brief Loads a scene and sets it as current active scene.
     * 
     * @param name The name of the scene.
     */
    void LoadScene(std::string name);

    /**
     * @brief Unloads a scene, freeing contained game objects and resources.
     * 
     * @param name The name of the scene.
     */
    void UnloadScene(std::string name);

    /**
     * @brief Get the Active Scene object
     * 
     * @return Scene* A pointer to the active Scene object.
     */
    Scene* GetActiveScene();

    /**
     * @brief Create a New Scene object. If no active scene exists, sets the created scene as the active scene.
     * 
     * @param name The name of the scene.
     * @return Scene* A pointer to a Scene object.
     */
    Scene* CreateNewScene(std::string name);

    /**
     * @brief Get the scene manager instance.
     * 
     * @return SceneManager* Pointer to the SceneManager instance.
     */
    static SceneManager* GetInstance();

};

} // namespace Engine::Core
