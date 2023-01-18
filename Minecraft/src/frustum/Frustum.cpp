#include "Frustum.h"

Frustum::Frustum(const glm::mat4 view_projection)
    : m_view_projection{view_projection}
{
}

void Frustum::update()
{
    // In glm we access elements as matrix[col][row] due to column-major layout
    //
    // Left clipping plane
    m_planes[Planes::LEFT].A = m_view_projection[0][3] + m_view_projection[0][0];
    m_planes[Planes::LEFT].B = m_view_projection[1][3] + m_view_projection[1][0];
    m_planes[Planes::LEFT].C = m_view_projection[2][3] + m_view_projection[2][0];
    m_planes[Planes::LEFT].D = m_view_projection[3][3] + m_view_projection[3][0];

    // Right clipping plane
    m_planes[Planes::RIGHT].A = m_view_projection[0][3] - m_view_projection[0][0];
    m_planes[Planes::RIGHT].B = m_view_projection[1][3] - m_view_projection[1][0];
    m_planes[Planes::RIGHT].C = m_view_projection[2][3] - m_view_projection[2][0];
    m_planes[Planes::RIGHT].D = m_view_projection[3][3] - m_view_projection[3][0];

    // Top clipping plane
    m_planes[Planes::TOP].A = m_view_projection[0][3] - m_view_projection[0][1];
    m_planes[Planes::TOP].B = m_view_projection[1][3] - m_view_projection[1][1];
    m_planes[Planes::TOP].C = m_view_projection[2][3] - m_view_projection[2][1];
    m_planes[Planes::TOP].D = m_view_projection[3][3] - m_view_projection[3][1];

    // Bottom clipping plane
    m_planes[Planes::BOTTOM].A = m_view_projection[0][3] + m_view_projection[0][1];
    m_planes[Planes::BOTTOM].B = m_view_projection[1][3] + m_view_projection[1][1];
    m_planes[Planes::BOTTOM].C = m_view_projection[2][3] + m_view_projection[2][1];
    m_planes[Planes::BOTTOM].D = m_view_projection[3][3] + m_view_projection[3][1];

    // Near clipping plane
    m_planes[Planes::NEAR].A = m_view_projection[0][3] + m_view_projection[0][2];
    m_planes[Planes::NEAR].B = m_view_projection[1][3] + m_view_projection[1][2];
    m_planes[Planes::NEAR].C = m_view_projection[2][3] + m_view_projection[2][2];
    m_planes[Planes::NEAR].D = m_view_projection[3][3] + m_view_projection[3][2];

    // Far clipping plane
    m_planes[Planes::FAR].A = m_view_projection[0][3] - m_view_projection[0][2];
    m_planes[Planes::FAR].B = m_view_projection[1][3] - m_view_projection[1][2];
    m_planes[Planes::FAR].C = m_view_projection[2][3] - m_view_projection[2][2];
    m_planes[Planes::FAR].D = m_view_projection[3][3] - m_view_projection[3][2];

    for (Plane plane : m_planes)
        plane.normalize();
}


bool Frustum::isPointInFrustum(const glm::vec3& world_point) const
{
    for (const Plane plane : m_planes)
    {
        if (plane.distanceToPoint(world_point) < 0)
            return false;
    }
    return true;
}

bool Frustum::isBoxInFrustum(const AABox& box) const
{
    for (const Plane plane : m_planes)
    {
        if (plane.distanceToPoint(box.getVertexPositiveTo(plane.getNormal())) < 0)
            return false;
    }
    return true;
}

void Frustum::setViewProjection(glm::mat4 view_projection)
{
    m_view_projection = view_projection;
}
