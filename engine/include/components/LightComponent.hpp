#pragma once
#include "components/BaseComponent.hpp"
#include "renderer/RenderingStructs.hpp"

#include "geometry/AxisAlignedBox.hpp"

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

    Rendering::PointLight* _pointLight = nullptr;
    Rendering::DirectionalLight* _directionalLight = new Rendering::DirectionalLight();
    // 
    // Rendering::DirectionalLight _directionalLight;
    glm::vec4 _colour = glm::vec4(1.0f);
    
    bool _debugDraw = false;

    LightType _type = LightType::DIRECTIONAL;

    void DebugDraw();

    void DebugGUI();

    void UpdateLight();
    
    Engine::Geometry::AxisAlignedBox* ViewFrustum;

public:

    LightComponent();

    void Update(float deltaTime) override;

    void SetColour(glm::vec4 colour);

    void SetType(LightType type);

    Rendering::PointLight& PointLight();

    Rendering::DirectionalLight& DirectionalLight();

    void SetDebugDrawDirectionEnabled(bool enabled = true);

    void DrawGUI();

    Engine::Geometry::AxisAlignedBox* GetViewFrustum();
};

} // namespace Components
