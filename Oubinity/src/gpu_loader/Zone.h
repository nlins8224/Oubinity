#pragma once
#include <vector>
#include "../chunk/Vertex.h"
#include "../renderer/ChunkRendererSettings.h"

namespace VertexPool {

// Ensure that max_vertices_per_bucket is always divisable by amount of vertices
// in a single face. This is needed to use gl_VertexID properly.
constexpr size_t floorToNearestNumberDivisableBy(size_t number,
                                                 size_t divisable_by) {
  return number - (number % divisable_by);
}

constexpr size_t vertices_in_zero_bucket =
    floorToNearestNumberDivisableBy(39996, Block::VERTICES_PER_FACE);
constexpr size_t vertices_in_one_bucket = floorToNearestNumberDivisableBy(
    vertices_in_zero_bucket / 4, Block::VERTICES_PER_FACE);
constexpr size_t vertices_in_two_bucket = floorToNearestNumberDivisableBy(
    vertices_in_one_bucket / 4, Block::VERTICES_PER_FACE);
constexpr size_t vertices_in_three_bucket = floorToNearestNumberDivisableBy(
    vertices_in_two_bucket / 4, Block::VERTICES_PER_FACE);
constexpr size_t vertices_in_four_bucket = floorToNearestNumberDivisableBy(
    vertices_in_three_bucket / 4, Block::VERTICES_PER_FACE);
constexpr size_t vertices_in_five_bucket = floorToNearestNumberDivisableBy(
    vertices_in_four_bucket / 4, Block::VERTICES_PER_FACE);

struct Zone {
  size_t level;
  size_t max_vertices_per_bucket;  // should be mod 6, because there are 6
                                   // vertices per each face
  size_t buckets_amount;  // calculated at ZoneVertexPool init, based on LOD
  size_t start_offset;    // ID of start Vertex
  size_t end_offset;      // ID of end Vertex
};

static Zone Zero{.level{0},
                 .max_vertices_per_bucket{vertices_in_zero_bucket},
                 .buckets_amount{0},
                 .start_offset{0},
                 .end_offset{0}};

static Zone One{.level{1},
                .max_vertices_per_bucket{vertices_in_one_bucket},
                .buckets_amount{0},
                .start_offset{0},
                .end_offset{0}};

static Zone Two{.level{2},
                .max_vertices_per_bucket{vertices_in_two_bucket},
                .buckets_amount{0},
                .start_offset{0},
                .end_offset{0}};

static Zone Three{.level{3},
                  .max_vertices_per_bucket{vertices_in_three_bucket},
                  .buckets_amount{0},
                  .start_offset{0},
                  .end_offset{0}};

static Zone Four{.level{4},
                 .max_vertices_per_bucket{vertices_in_four_bucket},
                 .buckets_amount{0},
                 .start_offset{0},
                 .end_offset{0}};

static Zone Five{.level{5},
                 .max_vertices_per_bucket{vertices_in_five_bucket},
                 .buckets_amount{0},
                 .start_offset{0},
                 .end_offset{0}

};

static std::vector<Zone*> zones{{&Zero},  {&One},  {&Two},
                                {&Three}, {&Four}, {&Five}};
}  // namespace VertexPool