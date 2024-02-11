#include "Chunk.h"

using Block::block_mesh;
using Block::block_id;


Chunk::Chunk(glm::ivec3 chunk_pos, LevelOfDetail::LevelOfDetail lod)
	: 
	m_chunk_pos{ chunk_pos },
	m_lod{ lod },
	m_world_pos{glm::vec3{chunk_pos.x * CHUNK_SIZE, chunk_pos.y * CHUNK_SIZE, chunk_pos.z * CHUNK_SIZE} },
	m_state{ ChunkState::NEW }
{
	m_is_terrain_generated = false;
	m_blocks = new Block::BlockArray();
	m_blocks->fill(Block::AIR);
}

Chunk::Chunk(const Chunk& chunk)
	:
	m_mesh{chunk.m_mesh},
	m_chunk_pos{chunk.m_chunk_pos},
	m_chunk_neighbors{chunk.m_chunk_neighbors},
	m_blocks{ chunk.m_blocks },
	m_world_pos{chunk.m_world_pos},
	m_is_terrain_generated{chunk.m_is_terrain_generated}
{

}

void Chunk::addChunkMesh()
{
	//if (m_chunk_neighbors.size() == 6) {
	//	LOG_F(INFO, "Chunk is surrounded, skipping");
	//	return;
	//}

	LOG_F(INFO, "Chunk neighbors amount: %d", m_chunk_neighbors.size());
	block_id block;
	for (int local_x = 0; local_x < m_lod.block_amount; local_x++)
	{
		for (int local_y = 0; local_y < m_lod.block_amount; local_y++)
		{
			for (int local_z = 0; local_z < m_lod.block_amount; local_z++)
			{
				block = getBlockId(glm::ivec3(local_x, local_y, local_z));
				if (block != block_id::AIR)
					addVisibleFaces(glm::ivec3(local_x, local_y, local_z));
			}
		}
	}

	// delete only when neighbors are in meshed state
	//delete m_blocks;
}

void Chunk::setBlock(glm::ivec3 block_pos, block_id type)
{
	m_blocks->set(block_pos, type);
}

glm::ivec3 Chunk::getPos() const
{
	return m_chunk_pos;
}

glm::ivec2 Chunk::getPosXZ() const
{
	return { m_chunk_pos.x, m_chunk_pos.z };
}

void Chunk::addVisibleFaces(glm::ivec3 block_pos)
{
	int x = block_pos.x, y = block_pos.y, z = block_pos.z;

	if (!isFaceVisible(glm::ivec3(x + 1, y, z))) addFace(block_mesh::RIGHT,  glm::ivec3(x, y, z));
	if (!isFaceVisible(glm::ivec3(x - 1, y, z))) addFace(block_mesh::LEFT,   glm::ivec3(x, y, z));
	if (!isFaceVisible(glm::ivec3(x, y + 1, z))) addFace(block_mesh::TOP,    glm::ivec3(x, y, z));
	if (!isFaceVisible(glm::ivec3(x, y - 1, z))) addFace(block_mesh::BOTTOM, glm::ivec3(x, y, z));
	if (!isFaceVisible(glm::ivec3(x, y, z + 1))) addFace(block_mesh::FRONT,  glm::ivec3(x, y, z));
	if (!isFaceVisible(glm::ivec3(x, y, z - 1))) addFace(block_mesh::BACK,   glm::ivec3(x, y, z));
}

// May overflow when near INT_MAX
// b parameter has to be positive
inline int roundDownDivide(int a, int b) {
	if (a >= 0) return a / b;
	else return (a - b + 1) / b;
}

// true modulo instead of C++ remainder modulo
inline int getMod(int pos, int mod)
{
	return ((pos % mod) + mod) % mod;
}

bool Chunk::isFaceVisible(glm::ivec3 block_pos) const
{
	int x = block_pos.x, y = block_pos.y, z = block_pos.z;
	// out of bounds check for example: x - 1 = -1 < 0, x + 1 = 32 > 31
	if (x < 0 || y < 0 || z < 0 || x >= m_lod.block_amount || y >= m_lod.block_amount || z >= m_lod.block_amount)
	{
		int c_x = roundDownDivide(x, m_lod.block_amount);
		int c_y = roundDownDivide(y, m_lod.block_amount);
		int c_z = roundDownDivide(z, m_lod.block_amount);

		glm::ivec3 neighbor_chunk_offset = glm::ivec3(c_x, c_y, c_z);
		glm::ivec3 neighbor_chunk_pos = m_chunk_pos + neighbor_chunk_offset;

		// One if statement, because there is FPS gain.
		// If LOD don't match, return false to have seamless transitions between lod levels
		// This creates additional "wall" and it costs a bit of FPS, but not much.
		if (m_chunk_neighbors.find(neighbor_chunk_pos) == m_chunk_neighbors.end() 
			|| m_chunk_neighbors.at(neighbor_chunk_pos)->getLevelOfDetail().block_amount != m_lod.block_amount) {
			return false;
		}

		int l_x = getMod(x, m_lod.block_amount);
		int l_y = getMod(y, m_lod.block_amount);
		int l_z = getMod(z, m_lod.block_amount);

		return m_chunk_neighbors.at(neighbor_chunk_pos)->getBlockId({l_x, l_y, l_z}) != block_id::AIR;
	}
	return m_blocks->get(glm::ivec3(x, y, z)) != block_id::AIR;
}

void Chunk::addFace(block_mesh face_side, glm::ivec3 block_pos)
{
	Block::block_id block_id{ getBlockId(block_pos) };

	// xyz are in local coords
	GLubyte x = static_cast<GLubyte>(block_pos.x);
	GLubyte y = static_cast<GLubyte>(block_pos.y);
	GLubyte z = static_cast<GLubyte>(block_pos.z);
	GLubyte texture_id{ static_cast<GLubyte>(block_id) };
	GLubyte face_id = static_cast<GLubyte>(face_side);

	FaceCornersAo corners_ao = calculateAmbientOcclusion(face_side, block_pos);

	Face face;
	face.packed_face = packFace(x, y, z, texture_id, face_id, corners_ao.top_left, corners_ao.top_right, corners_ao.bottom_right, corners_ao.bottom_left);
	
	for (GLubyte vertex_id = 0; vertex_id < Block::VERTICES_PER_FACE; vertex_id++)
	{
		Vertex vertex;
		vertex.packed_vertex = packVertex(x, y, z, texture_id, face_id);
		//LOG_F(INFO, "vertices ao: %d", vertices_ao[vertex_id]);
		m_mesh.addVertex(vertex);
	}
	m_faces.push_back(face);
	m_added_faces++;
}

FaceCornersAo Chunk::calculateAmbientOcclusion(block_mesh face_side, glm::ivec3 block_pos) {
	int x = block_pos.x, y = block_pos.y, z = block_pos.z;
	FaceCornersAo corners_ao;

	switch (face_side) {
		case block_mesh::RIGHT:
			corners_ao = calculateAoPlaneX({x + 1, y, z});
			break;
		case block_mesh::LEFT:
			corners_ao = calculateAoPlaneX({x - 1, y, z});
			break;
		case block_mesh::TOP:
			corners_ao = calculateAoPlaneY({x, y + 1, z});
			break;
		case block_mesh::BOTTOM:
			corners_ao = calculateAoPlaneY({x, y - 1, z});
			break;
		case block_mesh::FRONT:
			corners_ao = calculateAoPlaneZ({x, y, z + 1});
			break;
		case block_mesh::BACK:
			corners_ao = calculateAoPlaneZ({x, y, z - 1});
			break;
		default:
			LOG_F(ERROR, "This should not be reached!");
	}
	return corners_ao;
}

FaceCornersAo Chunk::calculateAoPlaneY(glm::ivec3 block_pos) {
	int x = block_pos.x, y = block_pos.y, z = block_pos.z;

	uint8_t top          = !isFaceVisible({ x,     y, z - 1 });
	uint8_t top_left	 = !isFaceVisible({ x - 1, y, z - 1 });
	uint8_t left         = !isFaceVisible({ x - 1, y, z     });
	uint8_t bottom_left  = !isFaceVisible({ x - 1, y, z + 1 });
	uint8_t bottom       = !isFaceVisible({ x,     y, z + 1 });
	uint8_t bottom_right = !isFaceVisible({ x + 1, y, z + 1 });
	uint8_t right	     = !isFaceVisible({ x + 1, y, z     });
	uint8_t top_right    = !isFaceVisible({ x + 1, y, z - 1 });

	uint8_t top_left_corner = left + top_left + top;
	uint8_t top_right_corner = top + top_right + right;
	uint8_t bottom_right_corner = right + bottom_right + bottom;
	uint8_t bottom_left_corner = bottom + bottom_left + left;

	return { top_left_corner, top_right_corner, bottom_right_corner, bottom_left_corner };
}

FaceCornersAo Chunk::calculateAoPlaneX(glm::ivec3 block_pos) {
	int x = block_pos.x, y = block_pos.y, z = block_pos.z;

	uint8_t top			 = !isFaceVisible({ x, y,     z - 1 });
	uint8_t top_left     = !isFaceVisible({ x, y - 1, z - 1 });
	uint8_t left		 = !isFaceVisible({ x, y - 1, z     });
	uint8_t bottom_left  = !isFaceVisible({ x, y - 1, z + 1 });
	uint8_t bottom		 = !isFaceVisible({ x, y ,    z + 1 });
	uint8_t bottom_right = !isFaceVisible({ x, y + 1, z + 1 });
	uint8_t right		 = !isFaceVisible({ x, y + 1, z	   });
	uint8_t top_right    = !isFaceVisible({ x, y + 1, z - 1 });

	uint8_t top_left_corner = left + top_left + top;
	uint8_t top_right_corner = top + top_right + right;
	uint8_t bottom_right_corner = right + bottom_right + bottom;
	uint8_t bottom_left_corner = bottom + bottom_left + left;

	return { top_left_corner, top_right_corner, bottom_right_corner, bottom_left_corner };
}

FaceCornersAo Chunk::calculateAoPlaneZ(glm::ivec3 block_pos) {
	int x = block_pos.x, y = block_pos.y, z = block_pos.z;

	uint8_t top			 = !isFaceVisible({ x - 1, y,     z });
	uint8_t top_left	 = !isFaceVisible({ x - 1, y - 1, z });
	uint8_t left		 = !isFaceVisible({ x,	   y - 1, z });
	uint8_t bottom_left  = !isFaceVisible({ x + 1, y - 1, z });
	uint8_t bottom	     = !isFaceVisible({ x + 1, y,     z });
	uint8_t bottom_right = !isFaceVisible({ x + 1, y + 1, z });
	uint8_t right		 = !isFaceVisible({ x,	   y + 1, z });
	uint8_t top_right	 = !isFaceVisible({ x - 1, y + 1, z });

	uint8_t top_left_corner = left + top_left + top;
	uint8_t top_right_corner = top + top_right + right;
	uint8_t bottom_right_corner = right + bottom_right + bottom;
	uint8_t bottom_left_corner = bottom + bottom_left + left;

	return { top_left_corner, top_right_corner, bottom_right_corner, bottom_left_corner };
}

Block::block_id Chunk::getBlockId(glm::ivec3 block_pos) const
{
	return m_blocks->get(block_pos);
}

bool Chunk::isTransparent(glm::ivec3 block_pos) const
{
	return Block::getBlockType(this->getBlockId(block_pos)).transparent;
}

bool Chunk::isTerrainGenerated() const
{
	return m_is_terrain_generated;
}

void Chunk::setIsTerrainGenerated(bool is_generated)
{
	m_is_terrain_generated = is_generated;
}

Mesh& Chunk::getMesh()
{
	return m_mesh;
}

std::vector<Face>& Chunk::getFaces()
{
	return m_faces;
}

void Chunk::setState(ChunkState state)
{
	m_state = state;
}

void Chunk::setNeighbors(ChunkNeighbors neighbors)
{
	m_chunk_neighbors = neighbors;
}

Block::BlockArray& Chunk::getBlockArray()
{
	return *m_blocks;
}

const glm::vec3 Chunk::getWorldPos() const
{
	return m_world_pos;
}

LevelOfDetail::LevelOfDetail Chunk::getLevelOfDetail()
{
	return m_lod;
}

unsigned int Chunk::getAddedFacesAmount()
{
	return m_added_faces;
}
