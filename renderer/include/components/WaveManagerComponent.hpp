#pragma once
#include "components/BaseComponent.hpp"
#include "renderer/Mesh.hpp"
#include "renderer/Material.hpp"
#include "renderer/Shader.hpp"
#include <glm/glm.hpp>

#include <vector>

namespace Rendering
{
class Renderer;    
}

namespace Components
{

enum class WaveType { DIRECTIONAL, CIRCULAR};
struct WaveSource
{
    WaveType type;
    float steepness = 0.0f;
    float amplitude = 0.0f;
    float wavelength = 0.0f;
    float frequency = 0.0f;
    float directionAngle = 0.0f;
    float speed = 0.0f;
    float phaseConstant = 0.0f;
    float turnSpeed = 0.0f;
    glm::vec2 center;
    
public:
    //Directional
    WaveSource(float amplitude, float waveLength, float speed, float dirAngle, float turnSpeed);
    //Circular
    WaveSource(float amplitude, float waveLength, float speed, glm::vec2 center);

    void Update(float deltaTime, float steepness, int waveCount);
};



class WaveManagerComponent : public BaseComponent
{
friend class Rendering::Renderer;
private:
    const uint32_t MAX_WAVES = 3;
    std::vector<WaveSource> _waveSources;
    float _waveSteepness = 0.0f;
    Rendering::Material* _waveMaterial = nullptr;
    float _totalTime = 0.0f;

    void UpdateUniforms();

public:

    WaveManagerComponent();

    void Update(float deltaTime) override;

    void DrawGUI();

    void AddCircularWave(glm::vec2 center, float amplitude, float wavelength, float speed);

    void AddDirectionalWave(float directionAngle, float amplitude, float wavelength, float speed, float turnSpeed = 0.0f);

    void SetGerstnerMaterial(Rendering::Material* mat);
};

} // namespace Components

