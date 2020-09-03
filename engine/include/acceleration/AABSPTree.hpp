#pragma once

#include "core/GameObject.hpp"
#include "geometry/AxisAlignedBox.hpp"

#include <vector>

namespace Engine::Acceleration
{

class AABSPTree
{
private:

    struct TreeNode
    {
        TreeNode* Left;
        TreeNode* Right;
    };

    struct LeafNode : public TreeNode
    {
        Engine::Core::GameObject* GO;
        Engine::Geometry::AxisAlignedBox* AABB;
    };

public:
    AABSPTree(std::vector<std::pair<Engine::Core::GameObject*,Engine::Geometry::AxisAlignedBox*>> input);

};

} // namespace Engine::Acceleration
