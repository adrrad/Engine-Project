#include "utilities/bvh/BVHAnimation.hpp"
#include "utilities/StringUtilities.hpp"
#include "utilities/Utilities.hpp"


namespace Utilities::BVH
{
    
BVHAnimation::BVHAnimation(std::string& bvhcontent)
{
    // Parser utility variables
    std::string currentJoint = "";
    bool endSite = false;


    auto lines = Utilities::Split(bvhcontent, "\n");

    for(std::string line : lines)
    {
        line = Utilities::Trim(line);
        if(line.size() == 0) continue;
        if(line == "HIERARCHY") continue;
        if(line == "MOTION") continue;
        if(line == "{") continue;

        std::vector<std::string> splitted = Utilities::Split(line, " ");

        // Number of frames
        if(splitted[0] == "Frames:")
        {
            m_numFrames = std::stoi(splitted[1]);
            continue;
        }
        // Frame time
        if(splitted[0] == "Frame" && splitted[1] == "Time:")
        {
            m_frameTime = std::stof(splitted[2]);
            continue;
        }
        // Joint information
        if(splitted[0] == "ROOT" || splitted[0] == "JOINT")
        {
            std::string jointName = splitted[1];
            if(currentJoint == "") m_parents.push_back(0);
            else m_parents.push_back(Utilities::IndexOf(m_jointLabels, currentJoint));
            currentJoint = jointName;
            m_jointLabels.push_back(jointName);
            continue;
        }
        // Joint XYZ offsets
        if(splitted[0] == "OFFSET")
        {
            if(!endSite)
            {
                float x = std::stof(splitted[1]), y = std::stof(splitted[2]), z = std::stof(splitted[3]);
                m_offsets.push_back({x, y, z});
            }
            continue;
        }

        if(splitted[0] == "CHANNELS")
        {
            // m_channels.push_back({ splitted[2], splitted[3], splitted[4]});
            m_channels.push_back( std::vector<std::string>(splitted.begin()+2, splitted.end()));
            
            continue;
        }

        if(splitted[0] == "End" && splitted[1] == "Site")
        {
            endSite = true;
            continue;
        }

        if(splitted[0] == "}")
        {
            if(endSite) endSite = false;
            else 
            {
                int currentIndex = Utilities::IndexOf(m_jointLabels, currentJoint);
                currentJoint = m_jointLabels[m_parents[currentIndex]];
            }
            continue;
        }
        // READ EACH POSE VALUES
        int i = 0;
        int joint_index = 0;
        
        for(std::string label : m_jointLabels)
        {
            int channels = m_channels[joint_index].size();
            // Root joint
            if(channels == 6)
            {
                float xPos = std::stof(splitted[i++]), yPos = std::stof(splitted[i++]), zPos = std::stof(splitted[i++]);
                m_positions.push_back({xPos, yPos, zPos});
            }
            float xRot = std::stof(splitted[i++]), yRot = std::stof(splitted[i++]), zRot = std::stof(splitted[i++]);
            m_rotations.push_back({xRot, yRot, zRot});
            joint_index++;
        }
    }
    // m_numJoints = m_jointLabels.size();
}

} // namespace Utilities::BVH
