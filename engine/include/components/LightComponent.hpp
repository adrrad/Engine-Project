#pragma once

#include "components/BaseComponent.hpp"
#include "rendering/RenderingStructs.hpp"

#include "geometry/AxisAlignedBox.hpp"

namespace Engine::Rendering
{
class Renderer;    
}
namespace Engine::Components
{

enum class LightType { DIRECTIONAL, POINT };

class LightComponent : public BaseComponent
{
friend class Rendering::Renderer;
private:

    Rendering::PointLight* m_pointLight = nullptr;
    Rendering::DirectionalLight* m_directionalLight = new Rendering::DirectionalLight();
    // 
    // Rendering::DirectionalLight m_directionalLight;
    glm::vec4 m_colour = glm::vec4(1.0f);

    bool m_debugDraw = false;

    LightType m_type = LightType::DIRECTIONAL;

    void DebugDraw();

    void DebugGUI();

    void UpdateLight();
    
    Engine::Geometry::AxisAlignedBox* ViewFrustum;

public:

    LightComponent();

    void Start() override;

    void Update(float deltaTime) override;

    void SetColour(glm::vec4 colour);

    void SetType(LightType type);

    Rendering::PointLight& PointLight();

    Rendering::DirectionalLight& DirectionalLight();

    void SetDebugDrawDirectionEnabled(bool enabled = true);

    void DrawInspectorGUI() override;

    std::shared_ptr<Utilities::JSON::JSONValue> Serialise() override;

    void Deserialise(std::shared_ptr<Utilities::JSON::JSONValue> json) override;

};

} // namespace Engine::Components

