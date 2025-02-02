#include "Chunk.h"
#include "../Util.h"
using Block::block_id;
using Block::block_mesh;

Chunk::Chunk(glm::ivec3 chunk_pos, LevelOfDetail::LevelOfDetail lod)
    : m_is_generation_running{false}, 
      m_chunk_pos{chunk_pos},
      m_lod{lod},
      m_world_pos{Util::chunkPosToWorldPos(chunk_pos)},
      m_state{},
      m_is_visible{true},
      m_blocks{nullptr},
      m_mesh{nullptr} {}

Chunk::~Chunk() {
  if (m_blocks != nullptr) {
    delete m_blocks;
  }
}

void Chunk::addChunkMesh() {
  addFaces();
  // No need to store blocks if chunk was not edited by a player.
  // Blocks will be regenerated on a fly
  if (!m_state.was_edited) {
    m_blocks->clearBlockIdCache();
    m_state.has_blocks = false;
  }
}

void Chunk::setIsGenerationRunning(bool is_running) {
  m_is_generation_running = is_running;
}

void Chunk::setBlock(glm::ivec3 block_pos, block_id type) {
  m_blocks->setRaw(block_pos, type);
}

glm::ivec3 Chunk::getPos() const { return m_chunk_pos; }

glm::ivec2 Chunk::getPosXZ() const { return {m_chunk_pos.x, m_chunk_pos.z}; }

bool Chunk::isNeighborBlockVisible(glm::ivec3 block_pos) const {
  std::weak_ptr<Chunk> neighbor_chunk = findNeighborChunk(block_pos);

  // If LOD don't match, return false to have seamless transitions between lod
  // levels This creates additional "wall" and it costs a bit of FPS, but not
  // much.
  if (!neighbor_chunk.lock() ||
      neighbor_chunk.lock()->getLevelOfDetail().block_amount != m_lod.block_amount ||
      !neighbor_chunk.lock()->isVisible()) {
    return false;
  }

  return neighbor_chunk.lock()->isBlockPresent(findNeighborBlockPos(block_pos));
}

void Chunk::addFaces() {
  clearFaces();
  m_mesh = std::make_unique<MeshData>();
  const uint64_t CS = m_lod.block_amount;
  const uint64_t CS_2 = CS * CS;

  const uint64_t CS_P = m_lod.block_amount + 2;
  const uint64_t CS_P2 = CS_P * CS_P;
  const uint64_t CS_P3 = CS_P * CS_P * CS_P;
  const uint64_t CS_LAST_BIT = CS_P - 1;
  const uint64_t BORDER_MASK = (1ULL | (1ULL << (CS_P - 1)));

  sul::dynamic_bitset<> padded_blocks_presence_cache =
      m_blocks->getPaddedOccupancyMask();
  std::vector<block_id> padded_blocks_id_cache =
      m_blocks->getPaddedBlockIdCache();

  int min_y = 0, max_y = CS_P;
  int min_x = 0, max_x = CS_P;
  int min_z = 0, max_z = CS_P;

  bool neighbor_visible = false;

  // 1.
  /*
    axis 0:
    axis_cols[0, CS_P2) - iterating over xz plane in y direction, top and bottom
    faces axis 1: axis_cols[CS_P2, 2CS_P2) - iterating over zy plane in x
    direction, left and right faces axis 2: axis_cols[2CS_P2, 3CS_P2) -
    iterating over xy plane in z direction, front and back faces
  */
  std::vector<uint64_t> axis_cols(CS_P2 * 3, 0);
  int bit_pos = 0;
  for (uint64_t y = 0; y < CS_P; y++) {
    for (uint64_t x = 0; x < CS_P; x++) {
      uint64_t zb = 0;
      for (uint64_t z = 0; z < CS_P; z++) {
        if (padded_blocks_presence_cache[bit_pos]) {
          axis_cols[(CS_P * z) + x] |= 1ULL << y;
          axis_cols[CS_P2 + (y * CS_P) + z] |= 1ULL << x;
          zb |= 1ULL << z;
        }
        bit_pos++;
      }
      axis_cols[(CS_P2 * 2) + (x * CS_P) + y] = zb;
    }
  }

  // 2.
  /*
  axis 0: Top and Bottom faces
    col[0, CS_P2)
    col_face_masks[0, CS_P2)
    col_face_masks[CS_P2, 2CS_P2)
  axis 1: Left and Right faces
    col[CS_P2, 2CS_P2)
col_face_masks[2CS_P2, 3CS_P2)
    col_face_masks[3CS_P2, 4CS_P2)
  axis 2: Front and Back faces
    col[2CS_P2, 3CS_P2)
    col_face_masks[4CS_P2, 5CS_P2)
    col_face_masks[5CS_P2, 6CS_P2)
  */
  for (int axis = 0; axis <= 2; axis++) {
    for (int i = 0; i < CS_P2; i++) {
      uint64_t col = axis_cols[(CS_P2 * axis) + i];
      m_mesh->col_face_masks[(CS_P2 * (axis * 2)) + i] =
          col & ~((col >> 1) | (1ULL << CS_LAST_BIT));
      m_mesh->col_face_masks[(CS_P2 * (axis * 2 + 1)) + i] =
          col & ~((col << 1) | 1ULL);
    }
  }

  // 3. Greedy meshing
  bit_pos = 0;
  for (uint8_t face = 0; face < 6; face++) {
    int axis = face / 2;
    int air_dir = face % 2 == 0 ? 1 : -1;

    memset(m_mesh->merged_forward, 0, CS_P2 * 8);

    for (int forward = 1; forward < CS_P - 1; forward++) {
      uint64_t bits_walking_right = 0;

      int forwardIndex = (forward * CS_P) + (face * CS_P2);

      memset(m_mesh->merged_right, 0, CS_P * 8);

      for (int right = 1; right < CS_P - 1; right++) {
        uint64_t bits_here =
            m_mesh->col_face_masks[forwardIndex + right] & ~BORDER_MASK;
        uint64_t bits_right =
            right >= CS ? 0 : m_mesh->col_face_masks[forwardIndex + right + 1];
        uint64_t bits_forward =
            forward >= CS ? 0
                          : m_mesh->col_face_masks[forwardIndex + right + CS_P];

        uint64_t bits_merging_forward =
            bits_here & bits_forward & ~bits_walking_right;
        uint64_t bits_merging_right = bits_here & bits_right;
        unsigned long bit_pos = 0;

        uint64_t copy_front = bits_merging_forward;
        while (copy_front) {
#ifdef _MSC_VER
          _BitScanForward64(&bit_pos, copy_front);
#else
          bit_pos = __builtin_ctzll(copy_front);
#endif

          copy_front &= ~(1ULL << bit_pos);
          int column = get_axis_i(axis, right, forward, bit_pos);
          int column_forward = get_axis_i(axis, right, forward + 1, bit_pos);
          if (padded_blocks_id_cache[column] == padded_blocks_id_cache[column_forward] &&
              compareAO(padded_blocks_id_cache, axis, forward, right,
                        bit_pos + air_dir, 1, 0)) {
            m_mesh->merged_forward[(right * CS_P) + bit_pos]++;
          } else {
            bits_merging_forward &= ~(1ULL << bit_pos);
          }
        }

        uint64_t bits_stopped_forward = bits_here & ~bits_merging_forward;
        while (bits_stopped_forward) {
#ifdef _MSC_VER
          _BitScanForward64(&bit_pos, bits_stopped_forward);
#else
          bit_pos = __builtin_ctzll(bits_stopped_forward);
#endif

          bits_stopped_forward &= ~(1ULL << bit_pos);
          block_id type =
              padded_blocks_id_cache[get_axis_i(axis, right, forward, bit_pos)];

          if ((bits_merging_right & (1ULL << bit_pos)) != 0 &&
              (m_mesh->merged_forward[(right * CS_P) + bit_pos] ==
               m_mesh->merged_forward[(right + 1) * CS_P + bit_pos]) &&
              (type == padded_blocks_id_cache[get_axis_i(axis, right + 1,
                                                         forward, bit_pos)] &&
               compareAO(padded_blocks_id_cache, axis, forward, right,
                         bit_pos + air_dir, 0, 1))) {
            bits_walking_right |= 1ULL << bit_pos;
            m_mesh->merged_right[bit_pos]++;
            m_mesh->merged_forward[(right * CS_P) + bit_pos] = 0;
            continue;
          }

          bits_walking_right &= ~(1ULL << bit_pos);

          GreedyQuad greedy_quad;
          greedy_quad.front =
              forward - m_mesh->merged_forward[(right * CS_P) + bit_pos];
          greedy_quad.left = right - m_mesh->merged_right[bit_pos];
          greedy_quad.up = bit_pos + (face % 2 == 0 ? 1 : 0);

          greedy_quad.right = right + 1;
          greedy_quad.back = forward + 1;

          greedy_quad.width = m_mesh->merged_right[bit_pos] + 1;
          greedy_quad.height =
              m_mesh->merged_forward[(right * CS_P) + bit_pos] + 1;

          FaceCornersAo ao = bakeAO(padded_blocks_id_cache, bit_pos, air_dir,
                                    axis, right, forward);

          m_mesh->merged_forward[(right * CS_P) + bit_pos] = 0;
          m_mesh->merged_right[bit_pos] = 0;

          addGreedyFace(greedy_quad, static_cast<block_mesh>(face), type, ao);
        }
      }
    }
  }
}

const uint64_t Chunk::get_axis_i(const int axis, const int x, const int y,
                                 const int z) {
  const uint64_t CS = m_lod.block_amount;
  const uint64_t CS_P = m_lod.block_amount + 2;
  const uint64_t CS_P2 = CS_P * CS_P;

  if (axis == 0)
    return y + (x * CS_P) + (z * CS_P2);
  else if (axis == 1)
    return x + (z * CS_P) + (y * CS_P2);
  else
    return z + (y * CS_P) + (x * CS_P2);
}

void Chunk::addGreedyFace(GreedyQuad greedy_quad, Block::block_mesh face_side,
                          block_id type, FaceCornersAo ao) {
  GLuint w = 0, h = 0;
  GLuint x = 0, y = 0, z = 0;
  Face greedy_face;
  switch (face_side) {
    case block_mesh::TOP:
    case block_mesh::BOTTOM:
      x = greedy_quad.left;
      y = greedy_quad.up;
      z = greedy_quad.front;
      w = greedy_quad.width;
      h = greedy_quad.height;
      break;
    case block_mesh::RIGHT:
    case block_mesh::LEFT:
      x = greedy_quad.up;
      y = greedy_quad.front;
      z = greedy_quad.left;
      w = greedy_quad.height;
      h = greedy_quad.width;
      break;
    case block_mesh::FRONT:
    case block_mesh::BACK:
      x = greedy_quad.front;
      y = greedy_quad.left;
      z = greedy_quad.up;
      w = greedy_quad.height;
      h = greedy_quad.width;
      break;
  }

  FaceCornersAo ao_r = ao;
  switch (face_side) {
    case block_mesh::TOP:
      ao_r.left_back = ao.left_front;
      ao_r.right_back = ao.left_back;
      ao_r.right_front = ao.right_back;
      ao_r.left_front = ao.right_front;
      break;
    case block_mesh::RIGHT:
    case block_mesh::LEFT:
      ao_r.left_back = ao.right_back;
      ao_r.right_back = ao.right_front;
      ao_r.right_front = ao.left_front;
      ao_r.left_front = ao.left_back;
    case block_mesh::FRONT:
    case block_mesh::BACK:
      ao_r.left_back = ao.left_front;
      ao_r.right_back = ao.right_front;
      ao_r.right_front = ao.right_back;
      ao_r.left_front = ao.left_back;
      break;
  }

  greedy_face.packed_face_one = packFaceOne(x, y, z, w, h);
  greedy_face.packed_face_two =
      packFaceTwo(face_side, type, ao_r.left_back, ao_r.right_back,
                  ao_r.right_front, ao_r.left_front);
  m_faces.push_back(greedy_face);
}

const int Chunk::vertexAO(uint8_t side_first, uint8_t side_second,
                          uint8_t corner) {
  return (side_first && side_second)
             ? 0
             : (3 - (side_first + side_second + corner));
}

const bool Chunk::compareAO(const std::vector<block_id>& voxels, int axis,
                            int forward, int right, int c, int forward_offset, 
                            int right_offset) {
  for (auto& ao_dir : ao_dirs) {
    bool block_first_present =
        voxels[get_axis_i(axis, right + ao_dir[0], forward + ao_dir[1], c)] !=
        Block::AIR;
    bool block_second_present =
        voxels[get_axis_i(axis, right + right_offset + ao_dir[0],
                          forward + forward_offset + ao_dir[1], c)] !=
        Block::AIR;
    if (block_first_present != block_second_present) {
      return false;
    }
  }
  return true;
}

FaceCornersAo Chunk::bakeAO(const std::vector<block_id>& voxels,
                            uint64_t bit_pos, int air_dir, uint64_t axis,
                            uint64_t right, uint64_t forward) {
  int c = bit_pos + air_dir;
  uint8_t ao_F =
      voxels[get_axis_i(axis, right, forward - 1, c)] != block_id::AIR;
  uint8_t ao_B =
      voxels[get_axis_i(axis, right, forward + 1, c)] != block_id::AIR;
  uint8_t ao_L =
      voxels[get_axis_i(axis, right - 1, forward, c)] != block_id::AIR;
  uint8_t ao_R =
      voxels[get_axis_i(axis, right + 1, forward, c)] != block_id::AIR;

  uint8_t ao_LFC =
      !ao_L && !ao_F &&
      voxels[get_axis_i(axis, right - 1, forward - 1, c)] != block_id::AIR;
  uint8_t ao_LBC =
      !ao_L && !ao_B &&
      voxels[get_axis_i(axis, right - 1, forward + 1, c)] != block_id::AIR;
  uint8_t ao_RFC =
      !ao_R && !ao_F &&
      voxels[get_axis_i(axis, right + 1, forward - 1, c)] != block_id::AIR;
  uint8_t ao_RBC =
      !ao_R && !ao_B &&
      voxels[get_axis_i(axis, right + 1, forward + 1, c)] != block_id::AIR;

  FaceCornersAo corners;
  corners.left_back = vertexAO(ao_L, ao_B, ao_LBC);
  corners.right_back = vertexAO(ao_R, ao_B, ao_RBC);
  corners.right_front = vertexAO(ao_R, ao_F, ao_RFC);
  corners.left_front = vertexAO(ao_L, ao_F, ao_LFC);
  return corners;
}

Block::block_id Chunk::getBlockId(glm::ivec3 block_pos) const {
  return m_blocks->getRaw(block_pos);
}

ChunkNeighbors& Chunk::getNeighbors() { return m_chunk_neighbors; }

bool Chunk::isTransparent(glm::ivec3 block_pos) const {
  return Block::getBlockType(this->getBlockId(block_pos)).transparent;
}

bool Chunk::isVisible() const { return m_is_visible; }

bool Chunk::isBlockPresent(glm::ivec3 block_pos) const {
  if (isBlockOutsideChunk(block_pos)) {
    return isNeighborBlockVisible(block_pos);
  };
  return m_blocks->isBlockPresent(block_pos);
}
  

bool Chunk::isBlockOutsideChunk(glm::ivec3 block_pos) const {
  int x = block_pos.x, y = block_pos.y, z = block_pos.z;
  int chunk_size_padding = m_lod.block_amount + 2;
  return x < 0 || y < 0 || z < 0 || x >= chunk_size_padding ||
         y >= chunk_size_padding || z >= chunk_size_padding;
}

void Chunk::setIsVisible(bool is_visible) { m_is_visible = is_visible; }

std::vector<Vertex>& Chunk::getMesh() { return m_vertices; }

std::vector<Face>& Chunk::getFaces() { return m_faces; }

void Chunk::setState(ChunkState state) { m_state = state; }

bool Chunk::isGenerationRunning() { return m_is_generation_running; }

ChunkState Chunk::getState() { return m_state; }

void Chunk::setNeighbors(ChunkNeighbors neighbors) {
  m_chunk_neighbors = neighbors;
}

std::weak_ptr<Chunk> Chunk::findNeighborChunk(glm::ivec3 block_pos) const {
  int x = block_pos.x, y = block_pos.y, z = block_pos.z;

  int c_x = Util::roundDownDivide(x, m_lod.block_amount);
  int c_y = Util::roundDownDivide(y, m_lod.block_amount);
  int c_z = Util::roundDownDivide(z, m_lod.block_amount);

  glm::ivec3 neighbor_chunk_offset = glm::ivec3(c_x, c_y, c_z);
  glm::ivec3 neighbor_chunk_pos = m_chunk_pos + neighbor_chunk_offset;

  for (const auto& [chunk_pos, chunk] : m_chunk_neighbors) {
    if (neighbor_chunk_pos == chunk_pos) {
      return chunk;
    }
  }
}

glm::ivec3 Chunk::findNeighborBlockPos(glm::ivec3 block_pos) const {
  int x = block_pos.x, y = block_pos.y, z = block_pos.z;

  int l_x = Util::getMod(x, m_lod.block_amount);
  int l_y = Util::getMod(y, m_lod.block_amount);
  int l_z = Util::getMod(z, m_lod.block_amount);
  return {l_x, l_y, l_z};
}

Block::BlockStorage& Chunk::getBlockArray() { return *m_blocks; }

void Chunk::setBlockArray() { m_blocks = new Block::BlockStorage(m_lod); }

void Chunk::setChunkHasBlocksState(bool has_blocks) {
  m_state.has_blocks = has_blocks;
}

void Chunk::setChunkEditedState(bool was_edited) {
  m_state.was_edited = was_edited;
}

void Chunk::setChunkNeedsLodUpdate(bool needs_lod_update) {
  m_state.needs_lod_update = needs_lod_update;
}

void Chunk::setLevelOfDetail(LevelOfDetail::LevelOfDetail lod) { m_lod = lod; }

void Chunk::clearFaces() {
  m_faces.clear();
}

const glm::vec3 Chunk::getWorldPos() const { return m_world_pos; }

LevelOfDetail::LevelOfDetail Chunk::getLevelOfDetail() { return m_lod; }

unsigned int Chunk::getAddedFacesAmount() { return m_faces.size(); }
