#pragma once
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

void SkyboxComponent::SetSkyboxTexture(Texture* skybox)
{
    _skybox = skybox;
}


} // namespace Components

