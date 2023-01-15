#pragma once
#include <glm/glm.hpp>
#include <array>
#include "AABox.h"
#include "Plane.h"

// Based on
// https://www.gamedevs.org/uploads/fast-extraction-viewing-frustum-planes-from-world-view-projection-matrix.pdf
// http://www.lighthouse3d.com/tutorials/view-frustum-culling/

class Frustum
{
public:
    Frustum(const glm::mat4 view_projection);
    ~Frustum() = default;
    void update();
    bool isPointInFrustum(const glm::vec3& clip_space_point) const;
    bool isBoxInFrustum(const AABox& box) const;
    void setViewProjection(glm::mat4 view_projection);

private:
    std::array<Plane, 6> m_planes;
    glm::mat4 m_view_projection;
};