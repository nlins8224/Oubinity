#include "Chunk.h"
#include <iostream>
using BlockMesh::faces, BlockMesh::block_mesh, BlockMesh::FACE_SIZE;
using Block::block_id;


Chunk::Chunk(TextureManager* texture_manager, chunk_pos position)
	: 
	m_texture_manager{ texture_manager },
	m_chunk_position{ position }
{

}

Chunk::Chunk(const Chunk& chunk)
	:
	m_has_mesh{chunk.m_has_mesh},
	m_mesh_vertex_positions{chunk.m_mesh_vertex_positions},
	m_chunk_position{chunk.m_chunk_position},
	m_loader{chunk.m_loader},
	m_blocks{chunk.m_blocks},
	m_texture_manager{chunk.m_texture_manager}
{

}

void Chunk::updateChunk()
{
	prepareChunkMesh();
	loadChunkMesh();
	renderChunk();
}

void Chunk::prepareChunkMesh()
{
	int block{ 0 };
	for (int local_x = 0; local_x < CHUNK_SIZE; local_x++)
	{
		for (int local_y = 0; local_y < CHUNK_SIZE; local_y++)
		{
			for (int local_z = 0; local_z < CHUNK_SIZE; local_z++)
			{
				block = getBlockId(local_x, local_y, local_z);
				if (block != block_id::AIR)
					addVisibleFaces(local_x, local_y, local_z);
			}
		}
	}
}

void Chunk::loadChunkMesh()
{
	m_loader.loadMesh(m_mesh_vertex_positions);
}


void Chunk::renderChunk()
{
	m_loader.bindVAO();
	m_texture_manager->loadTexture();
	// This could be moved to renderer later
	glDrawArrays(GL_TRIANGLES, 0, m_mesh_vertex_positions.size());
}

void Chunk::setBlock(int x, int y, int z, block_id type)
{
	m_blocks[x][y][z] = type;
}

chunk_pos Chunk::getChunkPos()
{
	return m_chunk_position;
}

void Chunk::addVisibleFaces(int x, int y, int z)
{
	if (!isFaceVisible(x + 1, y, z)) addFace(faces[block_mesh::RIGHT],  x, y, z);
	if (!isFaceVisible(x - 1, y, z)) addFace(faces[block_mesh::LEFT],   x, y, z);
	if (!isFaceVisible(x, y + 1, z)) addFace(faces[block_mesh::TOP],    x, y, z);
	if (!isFaceVisible(x, y - 1, z)) addFace(faces[block_mesh::BOTTOM], x, y, z);
	if (!isFaceVisible(x, y, z + 1)) addFace(faces[block_mesh::FRONT],  x, y, z);
	if (!isFaceVisible(x, y, z - 1)) addFace(faces[block_mesh::BACK],   x, y, z);
}

bool Chunk::isFaceVisible(int x, int y, int z)
{
	// out of bounds check for example...
	if (x < 0 || y < 0 || z < 0) return false; // ...x - 1 = -1 < 0
	if (x >= CHUNK_SIZE || y >= CHUNK_SIZE || z >= CHUNK_SIZE) return false; // ...x + 1 = 16 > 15
	return m_blocks[x][y][z] != block_id::AIR;
}

void Chunk::addFace(std::array<float, FACE_SIZE> const &face, int x, int y, int z)
{
	const uint8_t FACE_ROWS{ 6 };
	int block_id{ getBlockId(x, y, z) };
	int texture_id{ setFaceTexture(block_id) };

	for (int i = 0; i < FACE_ROWS; i++)
	{
		uint8_t x_coord = i * 6;
		uint8_t y_coord = (i * 6) + 1;
		uint8_t z_coord = (i * 6) + 2;
		uint8_t u_coord = (i * 6) + 3;
		uint8_t v_coord = (i * 6) + 4;
		
		float x_world_pos = m_chunk_position.x * CHUNK_SIZE + x + face[x_coord];
		float y_world_pos = m_chunk_position.y * CHUNK_SIZE + y + face[y_coord];
		float z_world_pos = m_chunk_position.z * CHUNK_SIZE + z + face[z_coord];

		m_mesh_vertex_positions.push_back(x_world_pos);
		m_mesh_vertex_positions.push_back(y_world_pos);
		m_mesh_vertex_positions.push_back(z_world_pos);
		m_mesh_vertex_positions.push_back(face[u_coord]);
		m_mesh_vertex_positions.push_back(face[v_coord]);
		m_mesh_vertex_positions.push_back(texture_id);
	}
}


int Chunk::setFaceTexture(int block_id)
{
	std::string texture = Block::getBlockType(block_id).texture;
	m_texture_manager->addTexture(texture);
	return m_texture_manager->getTextureIndex(texture);	
}

// This could be optimised. For now chunk cannot recognize if a face is not visible,
// because other neighboring chunk hides it.
int Chunk::getBlockId(int x, int y, int z)
{
	return m_blocks[x][y][z];
}
