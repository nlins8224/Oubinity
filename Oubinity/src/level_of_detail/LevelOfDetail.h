#pragma once
#include <unordered_map>
#include <vector>
#include "../Camera.h"
#include "../Settings.h"

namespace LevelOfDetail {

struct LevelOfDetail {
  int level;
  int block_amount;
  float block_size;
  int draw_distance;  // how many chunks away from the camera this LOD level
                      // should start being active
  int divide_factor;
};

static const LevelOfDetail Zero{.level{0},
                                .block_amount{Settings::CHUNK_SIZE},
                                .block_size{1.0f},
                                .draw_distance{0},
                                .divide_factor{1}};

static const LevelOfDetail One{.level{1},
                               .block_amount{Settings::CHUNK_SIZE / 2},
                               .block_size{2.0f},
                               .draw_distance{16},
                               .divide_factor{2}};

static const LevelOfDetail Two{.level{2},
                               .block_amount{Settings::CHUNK_SIZE / 4},
                               .block_size{4.0f},
                               .draw_distance{32},
                               .divide_factor{4}};

static const LevelOfDetail Three{.level{3},
                                 .block_amount{Settings::CHUNK_SIZE / 8},
                                 .block_size{8.0f},
                                 .draw_distance{64},
                                 .divide_factor{8}};

static const LevelOfDetail Four{.level{4},
                                .block_amount{Settings::CHUNK_SIZE / 16},
                                .block_size{16.0f},
                                .draw_distance{256},
                                .divide_factor{16}};

static const LevelOfDetail Five{.level{5},
                                .block_amount{Settings::CHUNK_SIZE / 32},
                                .block_size{32.0f},
                                .draw_distance{512},
                                .divide_factor{32}};

static const std::vector<LevelOfDetail> Lods{{Zero},  {One},  {Two},
                                             {Three}, {Four}, {Five}};

static uint16_t distanceToCameraInChunks(glm::ivec3 camera_pos,
                                         glm::ivec3 chunk_pos) {
  uint16_t distance_x = std::abs(camera_pos.x - chunk_pos.x);
  uint16_t distance_z = std::abs(camera_pos.z - chunk_pos.z);
  return std::max(distance_x, distance_z);
}

static LevelOfDetail chooseLevelOfDetail(glm::ivec3 camera_pos,
                                         glm::ivec3 chunk_pos) {
  uint16_t draw_distance = distanceToCameraInChunks(camera_pos, chunk_pos);
  // multiply by 2, because distance to camera is exactly in half of the segment
  draw_distance *= 2;

  if (draw_distance <= One.draw_distance) return Zero;
  if (draw_distance <= Two.draw_distance) return One;
  if (draw_distance <= Three.draw_distance) return Two;
  if (draw_distance <= Four.draw_distance) return Three;
  if (draw_distance <= Five.draw_distance) return Four;

  return Five;
}

static uint8_t getMaxLodLevel() {
  uint8_t chunks_amount = Settings::MAX_RENDERED_CHUNKS_IN_XZ_AXIS;
  if (chunks_amount <= One.draw_distance) return 0;
  if (chunks_amount <= Two.draw_distance) return 1;
  if (chunks_amount <= Three.draw_distance) return 2;
  if (chunks_amount <= Four.draw_distance) return 3;
  if (chunks_amount <= Five.draw_distance) return 4;

  return 5;
}
}  // namespace LevelOfDetail
