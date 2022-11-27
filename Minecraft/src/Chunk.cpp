#include "Chunk.h"
#include "ChunkManager.h"
using BlockMesh::faces, BlockMesh::block_mesh, BlockMesh::FACE_SIZE;
using Block::block_id;


Chunk::Chunk(TextureManager* texture_manager, glm::ivec3 chunk_pos, ChunkManager* chunk_manager)
	: 
	m_texture_manager{ texture_manager },
	m_chunk_pos{ chunk_pos },
	m_chunk_manager{ chunk_manager },
	m_world_pos{glm::ivec3{chunk_pos.x * CHUNK_SIZE_X, chunk_pos.y * CHUNK_SIZE_Y, chunk_pos.z * CHUNK_SIZE_Z} }
{

}

Chunk::Chunk(const Chunk& chunk)
	:
	m_is_visible{ chunk.m_is_visible },
	m_mesh{chunk.m_mesh},
	m_chunk_pos{chunk.m_chunk_pos},
	m_chunk_manager{chunk.m_chunk_manager},
	m_loader{chunk.m_loader},
	m_texture_manager{chunk.m_texture_manager},
	m_blocks{ chunk.m_blocks }
{

}

void Chunk::updateChunk()
{
	OPTICK_EVENT();
	if (m_is_visible)
		return;

	prepareChunkMesh();
	loadChunkMesh();

	m_is_visible = true;
}

void Chunk::prepareChunkMesh()
{
	OPTICK_EVENT();
	m_mesh.clear();

	int block;
	for (int local_x = 0; local_x < CHUNK_SIZE_X; local_x++)
	{
		for (int local_y = 0; local_y < CHUNK_SIZE_Y; local_y++)
		{
			for (int local_z = 0; local_z < CHUNK_SIZE_Z; local_z++)
			{
				
				block = getBlockId(glm::ivec3(local_x, local_y, local_z));
				if (block != block_id::AIR)
					addVisibleFaces(glm::ivec3(local_x, local_y, local_z));
			}
		}
	}
}

void Chunk::loadChunkMesh()
{
	OPTICK_EVENT();
	m_loader.loadMesh(m_mesh);
}


void Chunk::renderChunk()
{
	OPTICK_EVENT();
	m_loader.bindVAO();
	// This could be moved to renderer later
	uint8_t vertices_per_triangle{ 3 };
	uint64_t amount_of_triangles{ m_mesh.vertex_positions.size() / vertices_per_triangle };
	glDrawArrays(GL_TRIANGLES, 0, amount_of_triangles);
}

void Chunk::setBlock(glm::ivec3 block_pos, block_id type)
{
	OPTICK_EVENT();
	m_blocks[block_pos.x][block_pos.y][block_pos.z] = type;
}

glm::ivec3 Chunk::getPosition()
{
	OPTICK_EVENT();
	return m_chunk_pos;
}

void Chunk::addVisibleFaces(glm::ivec3 block_pos)
{
	OPTICK_EVENT();
	int x = block_pos.x, y = block_pos.y, z = block_pos.z;

	if (!isFaceVisible(glm::ivec3(x + 1, y, z))) addFace(block_mesh::RIGHT, glm::ivec3(x, y, z));
	if (!isFaceVisible(glm::ivec3(x - 1, y, z))) addFace(block_mesh::LEFT, glm::ivec3(x, y, z));
	if (!isFaceVisible(glm::ivec3(x, y + 1, z))) addFace(block_mesh::TOP, glm::ivec3(x, y, z));
	if (!isFaceVisible(glm::ivec3(x, y - 1, z))) addFace(block_mesh::BOTTOM, glm::ivec3(x, y, z));
	if (!isFaceVisible(glm::ivec3(x, y, z + 1))) addFace(block_mesh::FRONT, glm::ivec3(x, y, z));
	if (!isFaceVisible(glm::ivec3(x, y, z - 1))) addFace(block_mesh::BACK, glm::ivec3(x, y, z));
}

bool Chunk::isFaceVisible(glm::ivec3 block_pos)
{
	int x = block_pos.x, y = block_pos.y, z = block_pos.z;
	// out of bounds check for example: x - 1 = -1 < 0, x + 1 = 16 > 15
	if (x < 0 || y < 0 || z < 0 || x >= CHUNK_SIZE_X || y >= CHUNK_SIZE_Y || z >= CHUNK_SIZE_Z)
	{
		glm::ivec3 world_pos{ m_world_pos.x + x, m_world_pos.y + y, m_world_pos.z + z };
		return m_chunk_manager->getChunkBlockId(world_pos) != block_id::AIR;
	}
	return m_blocks[x][y][z] != block_id::AIR;
}

void Chunk::addFace(block_mesh face_side, glm::ivec3 block_pos)
{
	OPTICK_EVENT();
	const uint8_t FACE_ROWS{ 6 };
	Block::block_id block_id{ getBlockId(block_pos) };
	int texture_id{ static_cast<int>(block_id) };

	std::array<float, FACE_SIZE> face{ faces[face_side] };

	uint8_t x_coord;
	uint8_t y_coord;
	uint8_t z_coord;
	uint8_t u_coord;
	uint8_t v_coord;
	uint8_t shading_coord;

	float x_world_pos;
	float y_world_pos;
	float z_world_pos;

	for (uint8_t i = 0; i < FACE_ROWS; i++)
	{
		x_coord = i * 7;
		y_coord = (i * 7) + 1;
		z_coord = (i * 7) + 2;
		u_coord = (i * 7) + 3;
		v_coord = (i * 7) + 4;
		shading_coord = (i * 7) + 6;
		
		x_world_pos = m_world_pos.x + block_pos.x + face[x_coord];
		y_world_pos = m_world_pos.y + block_pos.y + face[y_coord];
		z_world_pos = m_world_pos.z + block_pos.z + face[z_coord];

		m_mesh.addVertex(x_world_pos, y_world_pos, z_world_pos);
		m_mesh.addTexture(face[u_coord], face[v_coord], texture_id);
		m_mesh.addShading(face[shading_coord]);
	}
}

Block::block_id Chunk::getBlockId(glm::ivec3 block_pos)
{
	OPTICK_EVENT();
	return m_blocks[block_pos.x][block_pos.y][block_pos.z];
}

bool Chunk::isTransparent(glm::ivec3 block_pos)
{
	OPTICK_EVENT();
	return Block::getBlockType(this->getBlockId(block_pos)).transparent;
}
