#pragma once
#include <glm/glm.hpp>

#define SETTING_USE_PRELOADED_HEIGHTMAP 1
#define SETTING_USE_PRELOADED_COLORMAP 1
// Using PRELOADED_HEIGHTMAP is prerequisite
// Blends preloaded and procedural heightmap
#define SETTING_USE_HEIGHTMAP_BLENDING 1


#define SETTING_TREES_ENABLED 1
// Using PRELOADED_HEIGHTMAP is prerequisite
#define SETTING_USE_PRELOADED_TREEMAP 1

// Produces mesh per each vertex on CPU and allocates a buffer
#define SETTING_USE_VERTEX_MESH 0

namespace Settings {
// Used to determine scale of a height map or color map -> world map mapping,
// i.e. setting this to {2.0, 2.0, 2.0} means that each pixel on a height map or
// color map corresponds to 2x2x2 area in the world map. Bilinear interpolation
// is used on XZ plane, while heightmap is scaled on Y axis
static constexpr glm::vec3 SETTING_SCALE = {2.0, 2.0, 2.0};

// Used to determine thickness of a surface layer, when using
// PRELOADED_COLORMAP.
static constexpr int SETTING_BLOCK_MARGIN = SETTING_SCALE.y;
static constexpr int SETTING_PRELOADED_UNDEGROUND_LAYER_DEPTH = 6;

static constexpr int CHUNK_SIZE = 32;
static constexpr int CHUNK_SIZE_PADDING = CHUNK_SIZE + 2;

static constexpr double SETTING_CAMERA_SPEED = 15.0;

static constexpr int MAX_RENDERED_CHUNKS_IN_XZ_AXIS = 128;
static constexpr int MAX_RENDERED_CHUNKS_IN_Y_AXIS = 16;

constexpr char PRELOADED_HEIGHTMAP[] = "assets/mountain_lake_heightmap.png";
constexpr char PRELOADED_COLORMAP[] = "assets/mountain_lake_colormap.png";
constexpr char PRELOADED_TREEMAP[] = "assets/treemap.png";
}  // namespace Settings
 