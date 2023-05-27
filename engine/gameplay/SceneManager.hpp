#pragma once

#include "EngineComponent.hpp"
#include "Scene.hpp"

#include <string>
#include <vector>



class EngineComponent;

namespace Gameplay
{

class SceneManager : public EngineSubsystem<SceneManager>
{
friend class EngineSubsystem<SceneManager>;
private:

    std::vector<Scene*> m_scenes;
    Scene* m_activeScene;

    /**
     * @brief Construct a new Scene Manager object
     * 
     */
    SceneManager();

public:

    void Initialise() override;

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

};

} // namespace Gameplay
