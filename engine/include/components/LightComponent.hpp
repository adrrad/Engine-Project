#pragma once

#include "components/BaseComponent.hpp"
#include "rendering/RenderingStructs.hpp"
#include "rendering/Framebuffer.hpp"
#include "rendering/Material.hpp"

#include <glm/gtc/matrix_transform.hpp>

namespace Engine::Rendering
{
class Renderer;
class Renderpass;
}

namespace Engine::Geometry
{
    class Volume;
}

namespace Engine::Components
{

enum class LightType { DIRECTIONAL, POINT };

class LightComponent : public BaseComponent
{
friend class Rendering::Renderer;
friend class Rendering::Renderpass;
private:
    Rendering::BufferRange m_lightBuffer;
    Rendering::PointLight* m_pointLight = nullptr;
    Rendering::DirectionalLight* m_directionalLight = nullptr;
    Rendering::Framebuffer* m_shadowmap = nullptr;
    
    // float m_fieldOfView; - a constant (45 degrees)
    // float m_aspectRatio; - a constant (1:1)
    float m_nearPlane = 0.01f;
    float m_farPlane = 500;
    float m_viewSize = 100;

    // Shadow mapping - information needed
    // Directional light: far plane, frustum size
    // Point light: far plane determined by radius, fov = 45

    bool m_debugDraw = false;

    LightType m_type = LightType::DIRECTIONAL;

    void DebugDraw();

    void DebugGUI();

    void UpdateLight();


public:

    inline glm::mat4 OrthographicProjectionMatrix();

    inline glm::mat4 PerspectiveProjectionMatrix();

    glm::mat4 ViewProjectionMatrix();

    Geometry::Volume* GetLightVolume();

    LightComponent();

    void Start() override;

    void Update(float deltaTime) override;

    void SetColour(glm::vec4 colour);

    void SetType(LightType type);

    LightType GetType();

    Rendering::PointLight& PointLight();

    Rendering::DirectionalLight& DirectionalLight();

    inline Rendering::Framebuffer* GetShadowmap() { return m_shadowmap; }

    void SetDebugDrawDirectionEnabled(bool enabled = true);

    void DrawInspectorGUI() override;

    std::shared_ptr<Utilities::JSON::JSONValue> Serialise() override;

    void Deserialise(std::shared_ptr<Utilities::JSON::JSONValue> json) override;

};

glm::mat4 LightComponent::OrthographicProjectionMatrix()
{
    float halfsize = m_viewSize*0.5f;
    return glm::ortho(-halfsize, halfsize, -halfsize, halfsize, m_nearPlane, m_farPlane);
}

glm::mat4 LightComponent::PerspectiveProjectionMatrix()
{
    return glm::perspective(45.0f, 1.0f, m_nearPlane, m_farPlane);
}


} // namespace Engine::Components

