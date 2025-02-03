#include "ProceduralGenerator.h"
#include "../../third_party/timer.h"

ProceduralGenerator::ProceduralGenerator(int world_seed, uint8_t water_height)
    : m_world_seed{world_seed}, m_water_height{water_height} {}

ProceduralGenerator::ProceduralGenerator()
    : m_world_seed{1337}, m_water_height{10} {}

ProceduralHeightMap ProceduralGenerator::generateHeightMap(Chunk& chunk) {
  glm::ivec2 chunk_pos_xz = chunk.getPosXZ();
  ProceduralHeightMap height_map =
      generateHeightMap(chunk.getPos(), chunk.getLevelOfDetail(),
                        NoiseSettings::TestSettings, m_world_seed);
  return height_map;
}

ProceduralHeightMap ProceduralGenerator::generateHeightMap(
    glm::ivec3 chunk_pos, LevelOfDetail::LevelOfDetail lod) {
  glm::ivec2 chunk_pos_xz = {chunk_pos.x, chunk_pos.z};
  ProceduralHeightMap height_map = generateHeightMap(
      chunk_pos, lod, NoiseSettings::TestSettings, m_world_seed);
  return height_map;
}

ProceduralHeightMap ProceduralGenerator::generateHeightMap(
    glm::ivec3 chunk_pos, LevelOfDetail::LevelOfDetail lod,
    NoiseSettings::Settings settings, int seed) {
  auto fnSimplex = FastNoise::New<FastNoise::Simplex>();
  auto fnFractal = FastNoise::New<FastNoise::FractalFBm>();
  auto fnScale = FastNoise::New<FastNoise::DomainScale>();

  fnFractal->SetSource(fnSimplex);
  fnFractal->SetOctaveCount(settings.octaves);
  fnFractal->SetLacunarity(settings.lacunarity);
  fnFractal->SetGain(settings.fractal_gain);
  fnFractal->SetWeightedStrength(settings.weighted_strength);

  fnScale->SetSource(fnFractal);
  fnScale->SetScale(lod.block_size);

  int block_amount_padding = lod.block_amount + 2;
  int LEFT_PADDING = 1;
  std::vector<float> data_out(Settings::CHUNK_SIZE_PADDING * Settings::CHUNK_SIZE_PADDING);
  fnScale->GenUniformGrid2D(
      data_out.data(), chunk_pos.x * lod.block_amount - LEFT_PADDING,
      chunk_pos.z * lod.block_amount - LEFT_PADDING,
                            block_amount_padding, block_amount_padding,
                            settings.frequency, seed);
  HeightMap height_map{};

  for (int x = 0; x < CHUNK_SIZE_PADDING; x++) {
    for (int z = 0; z < CHUNK_SIZE_PADDING; z++) {
      float height = data_out[z * block_amount_padding + x];
      height_map[x][z] = 200.0f;  // std::sqrt(height * height) * 220.0f;
    }
  }

  return height_map;
}

bool ProceduralGenerator::generateLayers(Chunk& chunk,
                                         ProceduralHeightMap height_map) {
  int block_size = chunk.getLevelOfDetail().block_size;
  int block_amount_padding = chunk.getLevelOfDetail().block_amount + 2;
  glm::ivec3 chunk_world_pos = chunk.getPos() * CHUNK_SIZE;

  bool anything_added = false;
  for (int x = 0; x < block_amount_padding; x++) {
    // TODO: y for loop should not be needed at all.
    for (int y = 1; y < block_amount_padding - 1; y++) {
      for (int z = 0; z < block_amount_padding; z++) {
        glm::ivec3 block_pos = {x, y, z};

        float surface_height = height_map[x][z];
        glm::ivec3 block_padded_pos = chunk_world_pos + (block_pos * block_size);
        // Y axis cuts through Y Chunk column. There is one surface and one chunk that it should cut. Do not adjust Y block coord here.
        glm::ivec3 block_world_pos = block_padded_pos - glm::ivec3(1, 0, 1) * block_size;
        if (surface_height > block_world_pos.y - block_size &&
            surface_height < block_world_pos.y + block_size) {
          chunk.setBlock(block_pos, Block::GRASS);
          anything_added = true;
        } else if (surface_height > block_world_pos.y + block_size) {
          chunk.setBlock(block_pos, Block::STONE);
          anything_added = true;
        } else {
          chunk.setBlock(block_pos, Block::AIR);
        }
      } 
    }
  }

  return anything_added;
}

TreePresenceMap ProceduralGenerator::generateTreePresenceMap(
    glm::ivec3 chunk_pos, LevelOfDetail::LevelOfDetail lod,
    NoiseSettings::Settings settings, int seed) {
  ProceduralHeightMap height_map =
      generateHeightMap(chunk_pos, lod, settings, seed);
  TreePresenceMap tree_presence_map{};
  for (int x = 0; x < lod.block_amount; x++) {
    for (int z = 0; z < lod.block_amount; z++) {
      float height_normalized = (height_map[x][z] / 2.0) + 0.5;
      tree_presence_map[x][z] =
          (int)(height_normalized * 1000) % (250 / (lod.divide_factor * 2)) ==
          0;
    }
  }
  return tree_presence_map;
}

void ProceduralGenerator::generateTrees(Chunk& chunk) {
  glm::ivec3 chunk_pos = chunk.getPos();
  LevelOfDetail::LevelOfDetail lod = chunk.getLevelOfDetail();
  TreePresenceMap tree_presence_map = generateTreePresenceMap(
      chunk_pos, lod, NoiseSettings::TreeSettings, m_world_seed);
  ProceduralHeightMap base_map = generateHeightMap(chunk);
  HeightMap height_map;
  for (int x = 0; x < lod.block_amount; x++) {
    for (int z = 0; z < lod.block_amount; z++) {
      float height_normalized = (base_map[x][z] / 2.0) + 0.5;
      height_map[x][z] = base_map[x][z];
      tree_presence_map[x][z] =
          (int)(height_normalized * 1000) % (250 / (lod.divide_factor * 2)) ==
          0;
    }
  }

  m_decoration_generator.generateTrees(chunk, height_map, tree_presence_map,
                                       m_water_height);
}

void ProceduralGenerator::generateTrees(Chunk& chunk,
                                        ProceduralHeightMap& height_map) {
  glm::ivec3 chunk_pos = chunk.getPos();
  LevelOfDetail::LevelOfDetail lod = chunk.getLevelOfDetail();
  TreePresenceMap tree_presence_map = generateTreePresenceMap(
      chunk_pos, lod, NoiseSettings::TreeSettings, m_world_seed);
  HeightMap height_map_2D;
  for (int x = 0; x < lod.block_amount; x++) {
    for (int z = 0; z < lod.block_amount; z++) {
      float height_normalized = (height_map[x][z] / 2.0) + 0.5;
      height_map_2D[x][z] = height_map[x][z];
      tree_presence_map[x][z] =
          (int)(height_normalized * 1000) % (250 / (lod.divide_factor * 2)) ==
          0;
    }
  }
  m_decoration_generator.generateTrees(chunk, height_map_2D, tree_presence_map,
                                       m_water_height);
}

uint8_t ProceduralGenerator::getWaterHeight() { return m_water_height; }