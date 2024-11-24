#pragma once
#include <glm/glm.hpp>

enum Planes { LEFT, RIGHT, TOP, BOTTOM, NEAR, FAR };

struct Plane {
  float A, B, C, D;  // plane coefficients
  void normalize();
  float distanceToPoint(glm::vec3 point) const;
  glm::vec3 getNormal() const;
};
