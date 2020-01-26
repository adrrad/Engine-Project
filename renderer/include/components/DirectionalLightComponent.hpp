#pragma once
#include "components/BaseComponent.hpp"
#include "renderer/RenderingStructs.hpp"
namespace Rendering
{
class Renderer;    
}
namespace Components
{

class DirectionalLightComponent : public BaseComponent
{
friend class Rendering::Renderer;
private:
    Rendering::DirectionalLight _directionalLight;
    glm::vec4 _colour = glm::vec4(1.0f);
    bool _debugDraw = false;

    void DebugDraw();

    void DebugGUI();

public:

    DirectionalLightComponent();

    void Update(float deltaTime) override;

    void SetColour(glm::vec4 colour);

    void SetDebugDrawDirectionEnabled(bool enabled = true);

    void DrawGUI();
};

} // namespace Components

