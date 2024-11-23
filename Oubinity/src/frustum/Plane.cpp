#include "Plane.h"

float Plane::distanceToPoint(glm::vec3 point) const
{
	return A * point.x + B * point.y + C * point.z + D;
}

glm::vec3 Plane::getNormal() const
{
    return glm::vec3(A, B, C);
}

void Plane::normalize()
{
    float magnitude = sqrt(A * A + B * B + C * C);
    A /= magnitude;
    B /= magnitude;
    C /= magnitude;
    D /= magnitude;
}