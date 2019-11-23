#include "components/MeshComponent.hpp"

namespace Components
{

MeshComponent::MeshComponent()
{

}

void MeshComponent::Update(float deltaTime)
{

}

void MeshComponent::DrawGUI()
{
    
}

void MeshComponent::SetMesh(Rendering::Mesh *mesh)
{
    _mesh = mesh;
}


} // namespace Components

