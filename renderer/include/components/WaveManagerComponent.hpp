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

struct WaveParticle
{
    glm::vec3 Position;
    glm::vec3 Normal;
};

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
    float lifespan = 0.0f;
    float range = 0.0f;
    glm::vec2 center;
    
public:
    //Circular
    WaveSource(float amplitude, float waveLength, float speed, float lifespan, float range, float dirAngle);
    //Directional
    WaveSource(float amplitude, float waveLength, float speed, float lifespan, float range, glm::vec2 center);

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
    std::vector<Rendering::SceneObject*> _floatingObjects;

    void UpdateUniforms();

    glm::vec3 GetParticlePosition(glm::vec2 pos);

    glm::vec3 GetParticleNormal(glm::vec2 pos);

public:

    WaveManagerComponent();

    void Update(float deltaTime) override;

    void DrawGUI();

    void AddCircularWave(glm::vec3 center, float power, float range, float lifespan);

    void AddDirectionalWave(float directionAngle, float power, float range, float lifespan);

    void SetGerstnerMaterial(Rendering::Material* mat);

    WaveParticle GetWaveParticle(glm::vec2 xzPosition);

    void AddFloatingObject(Rendering::SceneObject* object);
};

} // namespace Components

