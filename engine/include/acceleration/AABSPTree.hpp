#pragma once

#include "core/GameObject.hpp"
#include "geometry/AxisAlignedBox.hpp"

#include <vector>

namespace Engine::Acceleration
{
typedef std::pair<Engine::Core::GameObject*,Engine::Geometry::AxisAlignedBox*> GameObjectAABBPair;
class AABSPTree
{
private:
    //Utility struct, not used except for inheritance and saving memory
    //(Instead of making LeafNode derive from TreeNode.)
    struct Tree
    {
        Tree() = default;
        Engine::Geometry::AxisAlignedBox* BoundingBox;
    };

    struct TreeNode : public Tree
    {
        Tree* Left;
        Tree* Right;
        TreeNode(Tree* left, Tree* right, Engine::Geometry::AxisAlignedBox* boundingBox);
    };

    struct LeafNode : public Tree
    {
        Engine::Core::GameObject* GO;
        LeafNode(Engine::Core::GameObject* gameObject, Engine::Geometry::AxisAlignedBox* boundingBox);
    };

    Tree* m_root;

    Engine::Geometry::AxisAlignedBox CalculateBoundingBox(std::vector<GameObjectAABBPair> volumes);

    void SplitHeuristic(std::vector<GameObjectAABBPair> volumes, Engine::Geometry::AxisAlignedBox& box, int& axis, float& cut);

    Tree* ConstructTree(std::vector<GameObjectAABBPair> input);

public:
    AABSPTree(std::vector<GameObjectAABBPair> input);

};

} // namespace Engine::Acceleration
