#pragma once

#include "components/BaseComponent.hpp"
#include "rendering/RenderingStructs.hpp"
#include "rendering/Framebuffer.hpp"
#include "rendering/Material.hpp"
#include "rendering/Light.hpp"
#include "geometry/AxisAlignedBox.hpp"

namespace Engine::Rendering
{
class Renderer;
class Renderpass;
}
namespace Engine::Components
{

enum class LightType { DIRECTIONAL, POINT };

class LightComponent : public BaseComponent
{
friend class Rendering::Renderer;
friend class Rendering::Renderpass;
private:
    Rendering::LightBuffer m_lightBuffer;
    Rendering::PointLight* m_pointLight = nullptr;
    Rendering::DirectionalLight* m_directionalLight = nullptr;

    bool m_debugDraw = false;

    LightType m_type = LightType::DIRECTIONAL;

    void DebugDraw();

    void DebugGUI();

    void UpdateLight();

public:

    LightComponent();

    void Start() override;

    void Update(float deltaTime) override;

    void SetColour(glm::vec4 colour);

    void SetType(LightType type);

    LightType GetType();

    Rendering::PointLight& PointLight();

    Rendering::DirectionalLight& DirectionalLight();

    void SetDebugDrawDirectionEnabled(bool enabled = true);

    void DrawInspectorGUI() override;

    std::shared_ptr<Utilities::JSON::JSONValue> Serialise() override;

    void Deserialise(std::shared_ptr<Utilities::JSON::JSONValue> json) override;

};

} // namespace Engine::Components

