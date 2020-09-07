#pragma once
#include "geometry/Volume.hpp"

#include "renderer/RenderingStructs.hpp"

#include <glm/glm.hpp>

#include <vector>

namespace Engine::Geometry
{

class AxisAlignedBox : public Volume
{
private:


public:
    const glm::vec3 Min;
    const glm::vec3 Max;

    AxisAlignedBox(glm::vec3 min, glm::vec3 max);

    AxisAlignedBox(const AxisAlignedBox &other);

    bool IntersectsAxisAlignedBox(AxisAlignedBox* other) override;

    bool IntersectsSphere(Sphere* other) override;

    bool ContainsPoint(const Point& p);

    Volume* GetTransformed(glm::mat4 trs) override;

    static AxisAlignedBox* FromVertexSet(std::vector<Rendering::Vertex> &vertices);
};

} // namespace Engine::Geometry
