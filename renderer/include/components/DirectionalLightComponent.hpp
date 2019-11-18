#pragma once
#include "components/BaseComponent.hpp"
#include "renderer/RenderingStructs.hpp"

namespace Components
{

class DirectionalLightComponent : public BaseComponent
{
private:
    Rendering::DirectionalLight _directionalLight;
    glm::vec4 _colour = glm::vec4(1.0f);
public:

    DirectionalLightComponent(Rendering::SceneObject* sceneObject);

    void Update(float deltaTime) override;

    void SetColour(glm::vec4 colour);

};

} // namespace Components

