#pragma once
#include "geometry/Volume.hpp"

#include <vector>
#include <glm/glm.hpp>

namespace Geometry
{

class AxisAlignedBox;

class OrientedBox : public Volume
{
private:
    // https://gamedev.stackexchange.com/questions/44500/how-many-and-which-axes-to-use-for-3d-obb-collision-with-sat
    bool Separated(std::vector<glm::vec3> vsA, std::vector<glm::vec3> vsB, glm::vec3 axis);

    void CalculateCorners();

    std::vector<glm::vec3> m_corners;
public:
    glm::vec3 Center;
    glm::vec3 U, V, W;
    float Hu, Hv, Hw;

    OrientedBox(glm::vec3 center, glm::vec3 u, glm::vec3 v, glm::vec3 w, float hu, float hv, float hw);

    // OrientedBox(const OrientedBox &other);

    OrientedBox(const AxisAlignedBox &aab);

    OrientedBox(const AxisAlignedBox &aab, glm::mat4 trs);

    bool IntersectsAxisAlignedBox(AxisAlignedBox* other) override;

    bool IntersectsOrientedBox(OrientedBox* other) override;

    bool IntersectsSphere(Sphere* other) override;

    bool ContainsPoint(const Point& p) override;

    Volume* GetTransformed(glm::mat4 trs) override;

    inline std::vector<glm::vec3> GetCorners()
    {
        if(m_corners.size() == 0) CalculateCorners();
        return m_corners;
    }
};


} // namespace Geometry
