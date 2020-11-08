#include "acceleration/AABSPTree.hpp"
#include "utilities/MathUtils.hpp"

#include <iostream>

using namespace glm;
using namespace std;
namespace Engine::Acceleration
{
    typedef Engine::Geometry::AxisAlignedBox AxisAlignedBox;
    typedef Engine::Core::GameObject GameObject;

    AABSPTree::TreeNode::TreeNode(Tree* left, Tree* right, Engine::Geometry::AxisAlignedBox* boundingBox)
    {
        Left = left;
        Right = right;
        BoundingBox = boundingBox;
    }

    AABSPTree::LeafNode::LeafNode(Engine::Core::GameObject* gameObject, Engine::Geometry::AxisAlignedBox* boundingBox)
    {
        GO = gameObject;
        BoundingBox = boundingBox;
    }

    AxisAlignedBox AABSPTree::CalculateBoundingBox(std::vector<GameObjectAABBPair> volumes)
    {
        if(volumes.size() == 0) throw std::exception("No volumes to calculate BB on!");
        glm::vec3 min = volumes[0].second->Min;
        glm::vec3 max = volumes[0].second->Max;
        for(auto& pair : volumes)
        {
            min = Utilities::Min(min, pair.second->Min);
            max = Utilities::Max(max, pair.second->Max);
        }
        return AxisAlignedBox(min, max);
    }

    void AABSPTree::SplitHeuristic(std::vector<GameObjectAABBPair> volumes, AxisAlignedBox& box, int& axis, float& cut)
    {
        vec3 dims = box.Max - box.Min;
        if(dims.x >= dims.y && dims.x >= dims.z) axis = 0;
        else if(dims.y >= dims.x && dims.y >= dims.z) axis = 1;
        else axis = 2;
        cut = volumes[volumes.size()/2].second->Max[axis];//box.Min[axis] + dims[axis]/2.0f;
    }

    AABSPTree::Tree* AABSPTree::ConstructTree(std::vector<GameObjectAABBPair> input)
    {
        if(input.size() == 0) return nullptr;
        AxisAlignedBox box = CalculateBoundingBox(input);
        if(input.size() == 1) return new LeafNode(input[0].first, input[0].second);
        
        int axis;
        float cut;
        std::vector<GameObjectAABBPair> left;
        std::vector<GameObjectAABBPair> right;
        SplitHeuristic(input, box, axis, cut);
        for(auto& pair : input)
        {
            const float& min = pair.second->Min[axis];
            const float& max = pair.second->Max[axis];
            if(max >= cut) right.push_back(pair);
            else left.push_back(pair);
        }
        cout << "axis " << axis << " cut " << cut << " left " << left.size() << " right " << right.size() << endl;
        Tree* rightNode = ConstructTree(right);
        Tree* leftNode = ConstructTree(left);
        return new TreeNode(leftNode, rightNode, new AxisAlignedBox(box));
    }

    AABSPTree::AABSPTree(std::vector<GameObjectAABBPair> input)
    {
        m_root = ConstructTree(input);
    }


} // namespace Engine::Acceleration
