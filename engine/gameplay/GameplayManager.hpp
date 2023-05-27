#pragma once

#include "EngineComponent.hpp"
#include "ecs/ECSManager.hpp"

namespace Gameplay
{

class ComponentManager;
class SceneManager;

class GameplayManager : public EngineSubsystem<GameplayManager>
{
friend class EngineSubsystem<GameplayManager>;

public:
    void Update(float deltaTime) override;

    void Initialise() override;

    void SetupECS();

    void Start();

private:

    GameplayManager();

    ComponentManager* m_componentManager;
    SceneManager* m_sceneManager;
    ECSManager* m_ecsManager;
};

} // namespace Gameplay