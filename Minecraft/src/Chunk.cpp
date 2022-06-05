#include "Chunk.h"
#include <iostream> // debug
using BlockMesh::faces, BlockMesh::block_mesh, BlockMesh::FACE_SIZE;
using Block::block_id;


Chunk::Chunk(chunk_pos position) : m_chunk_position{ position }
{
	// TODO: Fill array with 0's in .h?
	initAirChunk();
}

Chunk::Chunk(const Chunk& chunk) :
	m_has_mesh{chunk.m_has_mesh},
	m_mesh_vertex_positions{chunk.m_mesh_vertex_positions},
	m_chunk_position{chunk.m_chunk_position},
	m_loader{chunk.m_loader},
	m_blocks{chunk.m_blocks}
{

}

bool Chunk::isFaceVisible(int x, int y, int z)
{
	// out of bounds?, for example x - 1 < 0, x + 1 = 16 > 15
	if (x < 0 || y < 0 || z < 0) return false;
	if (x >= 16 || y >= 16 || z >= 16) return false;
	return m_blocks[x][y][z] != block_id::AIR;
}

// "30" should have a name, it is amount_of_vertices_in_a_face 
void Chunk::addFace(std::array<float, FACE_SIZE> const &face, int x, int y, int z)
{
	// This is ugly and should be refactored.
	const uint8_t FACE_ROWS{ 6 };
	for (int i = 0; i < FACE_ROWS; i++)
	{
		uint8_t x_coord = i * 5;
		uint8_t y_coord = (i * 5) + 1;
		uint8_t z_coord = (i * 5) + 2;
		uint8_t u_coord = (i * 5) + 3;
		uint8_t v_coord = (i * 5) + 4;
		
		float x_world_pos = m_chunk_position.x * CHUNK_SIZE + x + face[x_coord];
		float y_world_pos = m_chunk_position.y * CHUNK_SIZE + y + face[y_coord];
		float z_world_pos = m_chunk_position.z * CHUNK_SIZE + z + face[z_coord];

		m_mesh_vertex_positions.push_back(x_world_pos);
		m_mesh_vertex_positions.push_back(y_world_pos);
		m_mesh_vertex_positions.push_back(z_world_pos);
		m_mesh_vertex_positions.push_back(face[u_coord]);
		m_mesh_vertex_positions.push_back(face[v_coord]);
	}
}

// Explaination of this method is needed.
void Chunk::addVisibleFaces(int x, int y, int z)
{
	if (!isFaceVisible(x + 1, y, z)) addFace(faces[block_mesh::RIGHT],  x, y, z);
	if (!isFaceVisible(x - 1, y, z)) addFace(faces[block_mesh::LEFT],   x, y, z);
	if (!isFaceVisible(x, y + 1, z)) addFace(faces[block_mesh::TOP],    x, y, z);
	if (!isFaceVisible(x, y - 1, z)) addFace(faces[block_mesh::BOTTOM], x, y, z);
	if (!isFaceVisible(x, y, z + 1)) addFace(faces[block_mesh::FRONT],  x, y, z);
	if (!isFaceVisible(x, y, z - 1)) addFace(faces[block_mesh::BACK],   x, y, z);
}

void Chunk::updateChunk()
{
	prepareChunkMesh();
	loadChunkMesh();
	renderChunk();
}

void Chunk::setBlock(int x, int y, int z, block_id type)
{
	m_blocks[x][y][z] = type;
}

void Chunk::prepareChunkMesh()
{
	int block{ 0 };
	int x{ 0 }, y{ 0 }, z{ 0 };
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
	std::cout << glGetError() << std::endl;
	glDrawArrays(GL_TRIANGLES, 0, m_mesh_vertex_positions.size());

}

chunk_pos Chunk::getChunkPos()
{
	return m_chunk_position;
}

//for debug
std::array<std::array<std::array<int, 16>, 16>, 16> Chunk::getBlocks()
{
	return m_blocks;
}


void Chunk::initAirChunk()
{
	for (int local_x = 0; local_x < CHUNK_SIZE; local_x++)
	{
		for (int local_y = 0; local_y < CHUNK_SIZE; local_y++)
		{
			for (int local_z = 0; local_z < CHUNK_SIZE; local_z++)
			{
				m_blocks[local_x][local_y][local_z] = block_id::AIR;
			}
		}
	}
}

// This will not work when another chunk hides face
int Chunk::getBlockId(int x, int y, int z)
{
	return m_blocks[x][y][z];
}
