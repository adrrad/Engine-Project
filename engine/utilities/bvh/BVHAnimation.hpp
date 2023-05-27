#pragma once

#include <string>
#include <vector>

#include <glm/glm.hpp>

#include "platform/io/File.hpp"

namespace Utilities::BVH
{
class BVHAnimation
{

private:
	std::string m_name;
	uint32_t m_numFrames;
	float m_frameTime;
	std::vector<std::string> m_jointLabels;
	std::vector<uint32_t> m_parents;
	std::vector<std::vector<std::string>> m_channels;
	std::vector<glm::vec3> m_offsets; 	// Count: number of joints
	std::vector<glm::vec3> m_positions; // Count: (1 or number of joints) * number of frames
	std::vector<glm::vec3> m_rotations;	// Count: number of joints * number of frames

public:

	BVHAnimation(std::string& bvhcontent);
};
} // namespace Utilities::BVH
