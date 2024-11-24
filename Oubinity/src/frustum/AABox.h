#pragma once
#include <glm/glm.hpp>

class AABox {
 public:
  AABox(glm::vec3 world_pos, glm::vec3 box_size);
  ~AABox() = default;
  glm::vec3 getVertexPositiveTo(const glm::vec3& normal) const;

 private:
  glm::vec3 m_corner_pos;
  glm::vec3 m_box_size;
};
