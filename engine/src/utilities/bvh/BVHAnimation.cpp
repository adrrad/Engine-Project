#include "utilities/bvh/BVHAnimation.hpp"
#include "utilities/StringUtilities.hpp"


namespace Engine::Utilities::BVH
{
    
BVHAnimation::BVHAnimation(std::string& bvhcontent)
{
    auto lines = Utilities::Split(bvhcontent, "\n");

    for(std::string line : lines)
    {
        // line.erase(std::remove_if(line.begin(), line.end(), ::isspace), line.end());
        line = Utilities::Trim(line);
        if(line.size() == 0) continue;
        if(line == "HIERARCHY") continue;
        if(line == "MOTION") continue;
        if(line == "{") continue;

        std::vector<std::string> splitted = Utilities::Split(line, " ");
        
    }
}

} // namespace Engine::Utilities::BVH
