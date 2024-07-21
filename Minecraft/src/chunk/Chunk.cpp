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
	m_is_visible = true;
}

Chunk::Chunk(const Chunk& chunk)
	:
	m_mesh{chunk.m_mesh},
	m_chunk_pos{chunk.m_chunk_pos},
	m_chunk_neighbors{chunk.m_chunk_neighbors},
	m_blocks{ chunk.m_blocks },
	m_world_pos{chunk.m_world_pos},
	m_is_visible{chunk.m_is_visible}
{

}

Chunk::~Chunk()
{
	delete m_blocks;
}

void Chunk::addChunkMesh()
{
	addFaces();
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

inline Chunk* findNeighborAt(glm::ivec3 target_chunk_pos, const ChunkNeighbors& chunk_neighbors)
{
	for (const auto& [chunk_pos, chunk] : chunk_neighbors) {
		if (target_chunk_pos == chunk_pos) {
			return chunk;
		}
	}
	return nullptr;
}


bool Chunk::isFaceVisible(glm::ivec3 block_pos) const
{
	int x = block_pos.x, y = block_pos.y, z = block_pos.z;

	if (y < 0 || y > m_lod.block_amount) {
		return true;
	}
	if (x < 0 || z < 0 || x >= m_lod.block_amount || y == m_lod.block_amount || z >= m_lod.block_amount)
	{
		return isNeighborFaceVisible(block_pos);
	}

	block_id block_type = m_blocks->get(glm::ivec3(x, y, z));
	return block_type != block_id::AIR;
}

bool Chunk::isNeighborFaceVisible(glm::ivec3 block_pos) const
{
	int x = block_pos.x, y = block_pos.y, z = block_pos.z;

	int c_x = roundDownDivide(x, m_lod.block_amount);
	int c_y = roundDownDivide(y, m_lod.block_amount);
	int c_z = roundDownDivide(z, m_lod.block_amount);

	glm::ivec3 neighbor_chunk_offset = glm::ivec3(c_x, c_y, c_z);
	glm::ivec3 neighbor_chunk_pos = m_chunk_pos + neighbor_chunk_offset;

	// One if statement, because there is FPS gain.
	// If LOD don't match, return false to have seamless transitions between lod levels
	// This creates additional "wall" and it costs a bit of FPS, but not much.
	Chunk* neighbor_chunk = findNeighborAt(neighbor_chunk_pos, m_chunk_neighbors);

	if (neighbor_chunk == nullptr
		|| neighbor_chunk->getLevelOfDetail().block_amount != m_lod.block_amount
		|| !neighbor_chunk->isVisible()) {
		return false;
	}

	int l_x = getMod(x, m_lod.block_amount);
	int l_y = getMod(y, m_lod.block_amount);
	int l_z = getMod(z, m_lod.block_amount);

	block_id neighbor_block = neighbor_chunk->getBlockId({ l_x, l_y, l_z });
	return neighbor_block != block_id::AIR;
}

void Chunk::addFaces()
{
	const uint64_t CS = m_lod.block_amount;
	const uint64_t CS_2 = CS * CS;

	const uint64_t CS_P = m_lod.block_amount + 2;
    const uint64_t CS_P2 = CS_P * CS_P;
    const uint64_t CS_P3 = CS_P * CS_P * CS_P;
    const uint64_t CS_LAST_BIT = CS_P - 1;
	const uint64_t BORDER_MASK = (1ULL | (1ULL << (CS_P - 1)));

	sul::dynamic_bitset<>& blocks_presence_cache = m_blocks->getOccupancyMask();
	sul::dynamic_bitset<> padded_blocks_presence_cache(CS_P3, 0);


	int bit_pos = 0;
	for (uint64_t y = 0; y < CS_P; y++)
	{
		for (uint64_t x = 0; x < CS_P; x++)
		{
			for (uint64_t z = 0; z < CS_P; z++)
			{
				if (z == 0 || z == CS_P - 1 || x == 0 || x == CS_P - 1 || y == 0 || y == CS_P - 1) {
					padded_blocks_presence_cache[bit_pos] = isNeighborFaceVisible(glm::ivec3{ x - 1, y - 1, z - 1 });
				}
				else {
					padded_blocks_presence_cache[bit_pos] = blocks_presence_cache[(z - 1) + (x - 1) * CS + (y - 1) * CS * CS];
				}
				bit_pos++;
			}
		}
	}

	/*
	  axis 0:
	  axis_cols[0, CS_P2) - iterating over xz plane in y direction, top and bottom faces
	  axis 1:
	  axis_cols[CS_P2, 2CS_P2) - iterating over zy plane in x direction, left and right faces
	  axis 2:
	  axis_cols[2CS_P2, 3CS_P2) - iterating over xy plane in z direction, front and back faces
	*/
	std::vector<uint64_t> axis_cols(CS_P2 * 3, 0);// [CS_P2 * 3] = { 0 };

	bit_pos = 0;
	for (uint64_t y = 0; y < CS_P; y++)
	{
		for (uint64_t x = 0; x < CS_P; x++)
		{
			uint64_t zb = 0;
			for (uint64_t z = 0; z < CS_P; z++)
			{
				if (padded_blocks_presence_cache[bit_pos])
				{
					axis_cols[(CS_P * z) + x] |= 1ULL << y;
					axis_cols[CS_P2 + (y * CS_P) + z] |= 1ULL << x;
					zb |= 1ULL << z;
				}
				bit_pos++;
			}
			axis_cols[(CS_P2 * 2) + (x * CS_P) + y] = zb;
		}
	}


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
	for (int axis = 0; axis <= 2; axis++)
	{
		for (int i = 0; i < CS_P2; i++)
		{
			uint64_t col = axis_cols[(CS_P2 * axis) + i];
			m_mesh.col_face_masks[(CS_P2 * (axis * 2)) + i] = col & ~((col >> 1) | (1ULL << CS_LAST_BIT));
			m_mesh.col_face_masks[(CS_P2 * (axis * 2 + 1)) + i] = col & ~((col << 1) | 1ULL);
		}
	}

	// Greedy meshing
	for (uint8_t face = 0; face < 6; face++) {
		int axis = face / 2;

		for (int forward = 1; forward < CS_P - 1; forward++) {
			int forwardIndex = (forward * CS_P) + (face * CS_P2);

			for (int right = 1; right < CS_P - 1; right++) {

				uint64_t bits_here = m_mesh.col_face_masks[forwardIndex + right] &~ BORDER_MASK;
				unsigned long bit_pos;

				uint64_t copy_front = bits_here;
				while (copy_front) {
					#ifdef _MSC_VER
					_BitScanForward64(&bit_pos, copy_front);
					#else
					bit_pos = __builtin_ctzll(copy_front);
					#endif

					copy_front &= ~(1ULL << bit_pos);
					addFaceBasedOnFaceDirection(face, right - 1, forward - 1, bit_pos - 1);
				}
			}
		}
	}
}

const int Chunk::get_axis_i(const int axis, const int a, const int b, const int c) {
	const int CS = m_lod.block_amount;
	const int CS_P = m_lod.block_amount + 2;
	const int CS_P2 = CS_P * CS_P;
	const int CS_P3 = CS_P * CS_P * CS_P;
	const int CS_LAST_BIT = CS_P - 1;

	if (axis == 0) return b + (a * CS_P) + (c * CS_P2);
	else if (axis == 1) return a + (c * CS_P) + (b * CS_P2);
	else return c + (b * CS_P) + (a * CS_P2);
}

void Chunk::addFaceBasedOnFaceDirection(int direction, int right, int forward, int bit_pos) {
	int x, y, z;
	if (direction == 0 || direction == 1)
	{
		x = right;
		y = bit_pos;
		z = forward;
	}
	else if (direction == 2 || direction == 3)
	{
		x = bit_pos;
		y = forward;
		z = right;
	}
	else
	{
		x = forward;
		y = right;
		z = bit_pos;
	}
	addFace(static_cast<block_mesh>(direction), glm::ivec3(x, y, z));
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
	
	#if SETTING_USE_VERTEX_MESH
	for (GLubyte vertex_id = 0; vertex_id < Block::VERTICES_PER_FACE; vertex_id++)
	{
		Vertex vertex;
		m_mesh.addVertex(vertex);
	}
	#endif
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

ChunkNeighbors& Chunk::getNeighbors()
{
	return m_chunk_neighbors;
}

bool Chunk::isTransparent(glm::ivec3 block_pos) const
{
	return Block::getBlockType(this->getBlockId(block_pos)).transparent;
}

bool Chunk::isVisible() const
{
	return m_is_visible;
}

void Chunk::setIsVisible(bool is_visible)
{
	m_is_visible = is_visible;
}

std::vector<Vertex>& Chunk::getMesh()
{
	return m_mesh.vertices;
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

Block::PaletteBlockStorage& Chunk::getBlockArray()
{
	return *m_blocks;
}

void Chunk::setBlockArray()
{
	m_blocks = new Block::PaletteBlockStorage(m_lod);
	//m_blocks->fill(Block::AIR);
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

sul::dynamic_bitset<> Chunk::getBlocksBitset()
{
	return m_blocks->getPaletteIndexStorage().data();
}
