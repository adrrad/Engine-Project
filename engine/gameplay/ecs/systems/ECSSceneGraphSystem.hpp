#pragma once

#include "../ECSSystembase.hpp"

namespace Acceleration
{
    class Octree;
}

namespace Gameplay
{
    
class ECSSceneGraphSystem : public ECSSystembase
{

public:
    ECSSceneGraphSystem();
    // ~ECSSceneGraphSystem();

    void Update(float deltaTime) override;

    Signature GetSystemSignature() override;

private:

    void Initialise();

    void UpdateSceneGraph();

    Acceleration::Octree* m_sceneGraph;

};



} // namespace Gameplay
