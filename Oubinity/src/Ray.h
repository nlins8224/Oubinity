#pragma once
#include <glm/glm.hpp>
#include <cmath>
#include <functional>
#include "renderer/ChunkRenderer.h"
#include <iostream>
class Ray {
 public:
  Ray(ChunkRenderer& world, glm::vec3 origin, glm::vec3 direction)
      : m_world{world}, m_origin{origin}, m_direction{direction} {
    m_world_block_pos = glm::round(origin);
    m_current_pos = origin;
  };

  bool step(std::function<void(glm::vec3, glm::vec3)> hit_callback);
  double getDistance();

 private:
  glm::vec3 m_origin;
  glm::vec3 m_direction;
  glm::vec3 m_world_block_pos{-500.0, -500.0, -500.0};
  glm::vec3 m_current_pos{0, 0, 0};

  double m_distance_travelled{0};
  ChunkRenderer& m_world;

  bool check(std::function<void(glm::vec3, glm::vec3)> hit_callback,
             double distance, glm::vec3 current_block_pos,
             glm::vec3 next_block_pos);
};
