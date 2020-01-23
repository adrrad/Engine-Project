#include "components/WaveManagerComponent.hpp"

#include "renderer/SceneObject.hpp"

#include <iostream>
#include <imgui.h>

using namespace std;

namespace Components
{


WaveSource::WaveSource(float amplitude, float waveLength, float speed, float lifespan, float range, float dirAngle)
{
    this->type = WaveType::DIRECTIONAL;
    this->amplitude = amplitude;
    this->wavelength = waveLength;
    this->frequency = 2.0f/waveLength;
    this->directionAngle = dirAngle;
    this->speed = speed;
    this->phaseConstant = speed * frequency;
    this->lifespan = lifespan;
    this->range = range;
}

WaveSource::WaveSource(float amplitude, float waveLength, float speed, float lifespan, float range, glm::vec2 center)
{
    this->type = WaveType::CIRCULAR;
    this->amplitude = amplitude;
    this->wavelength = waveLength;
    this->frequency = 2.0f/waveLength;
    this->speed = speed;
    this->phaseConstant = speed * frequency;
    this->lifespan = lifespan;
    this->center = center;
    this->range = range;
}


void WaveSource::Update(float deltaTime, float steepness, int waveCount)
{
    frequency = 2.0f/wavelength;
    phaseConstant = speed * frequency;
    //this->steepness = steepness/(frequency*amplitude);
}

void WaveManagerComponent::UpdateUniforms()
{
    _waveMaterial->GetUniform("u_waveCount")->i = _waveSources.size();
    for(uint32_t waveIndex = 0; waveIndex < _waveSources.size(); waveIndex++)
    {
        
        std::string uniformName = std::string("u_waveSources[") + std::to_string(waveIndex) + std::string("]");
        _waveMaterial->GetUniform(uniformName + ".type")->i =  (int)_waveSources[waveIndex].type;
        _waveMaterial->GetUniform(uniformName + ".Q")->f =  _waveSources[waveIndex].steepness;
        _waveMaterial->GetUniform(uniformName + ".A")->f =  _waveSources[waveIndex].amplitude;
        //_waveMaterial->GetUniform(uniformName + ".L")->f =  _waveSources[waveIndex].wavelength;
        _waveMaterial->GetUniform(uniformName + ".w")->f =  _waveSources[waveIndex].frequency;
        _waveMaterial->GetUniform(uniformName + ".directionAngle")->f =  _waveSources[waveIndex].directionAngle;
        //_waveMaterial->GetUniform(uniformName + ".S")->f =  _waveSources[waveIndex].speed;
        _waveMaterial->GetUniform(uniformName + ".pc")->f =  _waveSources[waveIndex].phaseConstant;
        //_waveMaterial->GetUniform(uniformName + ".lifespan")->f =  _waveSources[waveIndex].lifespan;
        //_waveMaterial->GetUniform(uniformName + ".range")->f =  _waveSources[waveIndex].range;
        if(_waveSources[waveIndex].type == WaveType::DIRECTIONAL)
        {
            _waveMaterial->GetUniform(uniformName + ".directionAngle")->f =  _waveSources[waveIndex].directionAngle;
        }
        else
        {
            _waveMaterial->GetUniform(uniformName + ".center")->f2 = _waveSources[waveIndex].center;
        }
    }
    _waveMaterial->UpdateUniforms();
}

WaveManagerComponent::WaveManagerComponent()
{

}

void WaveManagerComponent::Update(float deltaTime)
{
    _totalTime += deltaTime;
    for(auto& wave : _waveSources)
    {
        wave.Update(deltaTime, _waveSteepness, _waveSources.size());
    }

    for(auto object : _floatingObjects)
    {
        glm::vec3 objectPosition = object->transform.position;
        glm::vec3 objectFwdVec = object->transform.GetDirection();
        WaveParticle particle = GetWaveParticle({objectPosition.x, objectPosition.z});

        object->transform.position = particle.Position;
        // glm::vec3 sideVec = glm::cross(objectFwdVec, -particle.Normal);
        // glm::vec3 newFwdVec = glm::cross(particle.Normal, sideVec);
        // glm::vec3 at = object->transform.position+objectFwdVec;
        // object->transform.LookAt(at, {0.0f,1.0f,0.0f});
    }
}

void WaveManagerComponent::DrawGUI()
{
    ImGui::Begin("Settings");
    ImGui::Text("Waves");
    ImGui::DragFloat("Steepness", &_waveSteepness, 0.05f, 0.0f, 1.0f);
    int i = 0;
    for(auto& wave : _waveSources)
    {
        ImGui::PushID(i);
        auto num = std::to_string(i);
        ImGui::Text((string("Wave ") + num).c_str());
        i++;
        ImGui::DragFloat("steepness ", &wave.steepness, 0.1f, 0.0f, 1.0f);
        ImGui::DragFloat("amplitude ", &wave.amplitude, 0.1f);
        ImGui::DragFloat("wavelength ", &wave.wavelength, 0.1f);
        ImGui::DragFloat("speed ", &wave.speed, 0.1f);
        ImGui::DragFloat("lifespan ", &wave.lifespan, 0.1f);
        if(wave.type == WaveType::CIRCULAR)
        {
            ImGui::DragFloat2("center ", &wave.center[0], 0.1f);
        }
        else
        {
            ImGui::DragFloat("direction angle ", &wave.directionAngle, 0.1f);
        }
        
        ImGui::DragFloat("range ", &wave.range, 0.1f);
        ImGui::PopID();
    }
    ImGui::End();
}

void WaveManagerComponent::AddCircularWave(glm::vec3 center, float power, float range, float lifespan)
{
    //dynamic steepness (the further the wave gets, the more it grows)
    //dynamic amplitude: center: max -> edge: 0
    //dynamic wavelength like amplitude?
    //constant speed

    WaveSource src = WaveSource(0.5f, 10.0f, 10.0f, 1.0f, 100.0f, center);
    _waveSources.push_back(src);
}

void WaveManagerComponent::AddDirectionalWave(float directionAngle, float power, float range, float lifespan)
{
    WaveSource src = WaveSource(0.5f, 10.0f, 10.0f, 1.0f, 100.0f, directionAngle);
    _waveSources.push_back(src);
}

void WaveManagerComponent::SetGerstnerMaterial(Rendering::Material* mat)
{
    _waveMaterial = mat;
}

glm::vec3 WaveManagerComponent::GetParticlePosition(glm::vec2 pos)
{
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
    for(uint32_t waveIndex = 0; waveIndex < _waveSources.size(); waveIndex++)
    {
        WaveType type = _waveSources[waveIndex].type;
        float A = _waveSources[waveIndex].amplitude;
        float Q = _waveSources[waveIndex].steepness;
        float angle = _waveSources[waveIndex].directionAngle;
        float w = _waveSources[waveIndex].frequency;
        float pc = _waveSources[waveIndex].phaseConstant;
        glm::vec2 dir;
        if(type == WaveType::DIRECTIONAL)
        {
            dir = glm::vec2(cos(angle), sin(angle));
        }
        else
        {
            dir = glm::normalize(_waveSources[waveIndex].center - pos);
        }
        x += pos.x + (Q * A * dir.x * cos(w * glm::dot(dir, pos) + pc * _totalTime));
        z += pos.y + (Q * A * dir.y * cos(w * glm::dot(dir, pos) + pc * _totalTime));
        y += A * sin(glm::dot(w*dir, pos) + pc * _totalTime);
    }
    return {x,y,z};
}

glm::vec3 WaveManagerComponent::GetParticleNormal(glm::vec2 pos)
{
    glm::vec3 position = {pos.x, 0.0f, pos.y};
    glm::vec3 dx = position + glm::vec3(-0.01, 0, 0);
    glm::vec3 dz = position + glm::vec3(0, 0, -0.01);
    dx = GetParticlePosition({dx.x, dx.z});
    dz = GetParticlePosition({dz.x, dz.z});
    glm::vec3 normal = glm::normalize(glm::cross(position - dz, position - dx));
    return normal;
}

WaveParticle WaveManagerComponent::GetWaveParticle(glm::vec2 xzPosition)
{
    glm::vec3 position = GetParticlePosition(xzPosition);
    glm::vec3 normal = GetParticleNormal(xzPosition);

    return {position, normal};
}

void WaveManagerComponent::AddFloatingObject(Rendering::SceneObject* object)
{
    _floatingObjects.push_back(object);
}

} // namespace Components

