#include "utils.hpp"

using namespace std;
using namespace glm;
using namespace Engine;
using namespace Engine::Rendering;
using namespace Engine::Components;
using namespace Engine::Utilities;
using namespace Engine::Core;
using namespace Engine::Geometry;
using namespace Engine::Acceleration;

Texture* lightbulbIcon;
Scene scene = Scene();


int scene2()
{
    Platform::IO::Directory dir(Platform::IO::Path(std::string(RESOURCES_DIR)), true);
    EngineSettings settings;
    settings.Project.ProjectName = "Engine";
    settings.Window.Width = 1600;
    settings.Window.Height = 1024;
    settings.Window.Maximized = false;
    Engine::Assets::AssetManager manager(Platform::IO::Path(std::string(PROJECT_DIR)));

    Engine::Editor::EditorCore editor(settings);
    Renderer* renderer = Renderer::GetInstance();
    Engine::Physics::PhysicsManager* physicsManager = Engine::Physics::PhysicsManager::GetInstance();
    
    ComponentManager::RegisterComponentPool<CameraComponent>();
    ComponentManager::RegisterComponentPool<InspectorCameraComponent>();
    ComponentManager::RegisterComponentPool<MeshComponent>();
    ComponentManager::RegisterComponentPool<RigidBodyComponent>();
    ComponentManager::RegisterComponentPool<LightComponent>();
    ComponentManager::RegisterComponentPool<MovementComponent>();
    // Platform::IO::File file = Platform::IO::File(RESOURCES_DIR+string("JSONTEST.json"));
    // string s = string(file.ReadAll().Data(), file.Size);
    // auto& scene_info = *JSON::ParseJSON(s);
    // Scene scenetest;
    // Utilities::Serialisation::DeserialiseObject<Scene>(&scenetest, scene_info);
    physicsManager->SetDebugDraw(true);
    editor.SetCurrentScene(&scene);

    lightbulbIcon = Utilities::ImportTexture(GetAbsoluteResourcesPath("\\icons\\lightbulb_outline.png"), GL_TEXTURE_2D);

    editor.Run();
    return 0;
}

int main()
{
    scene2();
}