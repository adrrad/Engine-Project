#pragma once
#include "components/BaseComponent.hpp"
#include "renderer/RenderingStructs.hpp"
namespace Rendering
{
class Renderer;    
}
namespace Components
{

enum class LightType { DIRECTIONAL, POINT };

class LightComponent : public BaseComponent
{
friend class Rendering::Renderer;
private:
    union
    {
        Rendering::PointLight _pointLight;
        Rendering::DirectionalLight _directionalLight;
    };
    // 
    // Rendering::DirectionalLight _directionalLight;
    glm::vec4 _colour = glm::vec4(1.0f);
    
    bool _debugDraw = false;

    LightType _type = LightType::DIRECTIONAL;

    void DebugDraw();

    void DebugGUI();

    void UpdateLight();
    
public:


    LightComponent();

    void Update(float deltaTime) override;

    void SetColour(glm::vec4 colour);

    void SetType(LightType type);

    Rendering::PointLight& PointLight();

    Rendering::DirectionalLight& DirectionalLight();

    void SetDebugDrawDirectionEnabled(bool enabled = true);

    void DrawGUI();
};

} // namespace Components

