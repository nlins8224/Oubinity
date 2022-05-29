#include "Chunk.h"
using BlockMesh::faces, BlockMesh::block_mesh;
using Block::block_id;

Chunk::Chunk(glm::ivec3 position) : m_chunk_position{ position }
{
	// TODO: Fill array with 0's in .h?
	initAirChunk();
}

Chunk::Chunk(const Chunk& chunk) :
	m_has_mesh{chunk.m_has_mesh},
	m_chunk_loader{chunk.m_chunk_loader},
	m_mesh_vertex_positions{chunk.m_mesh_vertex_positions},
	m_chunk_position{chunk.m_chunk_position},
	m_blocks{chunk.m_blocks}
{

}

bool Chunk::isFaceVisible(int x, int y, int z)
{
	// quick hack, translate coords later
	if (x < 0 || y < 0 || z < 0) return false;
	// out of bounds?, for example x + 1 = 16 > 15
	if (x >= 16 || y >= 16 || z >= 16) return false;
	return m_blocks[x][y][z] != block_id::AIR;
}

// "30" should have a name, it is amount_of_vertices_in_a_face 
void Chunk::addFace(std::array<float, 30> const &face, int x, int y, int z)
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

		m_mesh_vertex_positions.push_back(face[x_coord] + x + m_chunk_position.x * CHUNK_SIZE);
		m_mesh_vertex_positions.push_back(face[y_coord] + y + m_chunk_position.y * CHUNK_SIZE);
		m_mesh_vertex_positions.push_back(face[z_coord] + z + m_chunk_position.z * CHUNK_SIZE);
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
				if (block == block_id::AIR) continue;

				addVisibleFaces(local_x, local_y, local_z);
			}
		}
	}
}

// This will be moved later to Loader class
// Should be something like m_chunk_loader.load(m_mesh_vertex_positions);
void Chunk::loadChunkMesh()
{
	unsigned int VBO, VAO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, m_mesh_vertex_positions.size() * sizeof(float), &m_mesh_vertex_positions.front(), GL_STATIC_DRAW);

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
}


void Chunk::renderChunk()
{
	glDrawArrays(GL_TRIANGLES, 0, m_mesh_vertex_positions.size());
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
