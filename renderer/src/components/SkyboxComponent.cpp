#pragma once

#include "renderer/Renderer.hpp"
#include "components/SkyboxComponent.hpp"
#include "utilities/Utilities.hpp"

using namespace Rendering;
using namespace Utilities;
using namespace std;
namespace Components
{

SkyboxComponent::SkyboxComponent()
{
    string vPath = GetAbsoluteResourcesPath("\\shaders\\Skybox.vert");
    string fPath = GetAbsoluteResourcesPath("\\shaders\\Skybox.frag");
    Shader* shader = new Shader(vPath, fPath);
    _material = new Material(shader);
    _cube = Mesh::GetSkybox(shader);
}

void SkyboxComponent::Update(float deltaTime)
{
    
}

void SkyboxComponent::SetSkyboxTexture(Texture* skyboxTexture)
{
    _skyboxTexture = skyboxTexture;
    SetSkybox();
}

void SkyboxComponent::SetSkybox()
{
    _skybox.SkyboxMesh = _cube;
    _skybox.SkyboxMaterial = _material;
    _skybox.SkyboxTexture = _skyboxTexture;
    Renderer::GetInstance()->SetSkybox(&_skybox);
}

} // namespace Components
