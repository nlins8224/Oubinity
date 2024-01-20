#include "Chunk.h"

using Block::block_mesh;
using Block::block_id;


Chunk::Chunk(glm::ivec3 chunk_pos, LevelOfDetail::LevelOfDetail lod)
	: 
	m_chunk_pos{ chunk_pos },
	m_lod{ lod },
	m_world_pos{glm::vec3{chunk_pos.x * CHUNK_SIZE, chunk_pos.y * CHUNK_SIZE, chunk_pos.z * CHUNK_SIZE} }
{
	m_is_terrain_generated = false;
	m_blocks = new Block::BlockArray();
}

Chunk::Chunk(const Chunk& chunk)
	:
	m_mesh{chunk.m_mesh},
	m_chunk_pos{chunk.m_chunk_pos},
	m_blocks{ chunk.m_blocks },
	m_world_pos{chunk.m_world_pos},
	m_is_terrain_generated{chunk.m_is_terrain_generated}
{

}

void Chunk::addChunkMesh()
{
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

	delete m_blocks;
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

	if (!isFaceVisible(glm::ivec3(x + 1, y, z))) addFace(block_mesh::RIGHT, glm::ivec3(x, y, z));
	if (!isFaceVisible(glm::ivec3(x - 1, y, z))) addFace(block_mesh::LEFT, glm::ivec3(x, y, z));
	if (!isFaceVisible(glm::ivec3(x, y + 1, z))) addFace(block_mesh::TOP, glm::ivec3(x, y, z));
	if (!isFaceVisible(glm::ivec3(x, y - 1, z))) addFace(block_mesh::BOTTOM, glm::ivec3(x, y, z));
	if (!isFaceVisible(glm::ivec3(x, y, z + 1))) addFace(block_mesh::FRONT, glm::ivec3(x, y, z));
	if (!isFaceVisible(glm::ivec3(x, y, z - 1))) addFace(block_mesh::BACK, glm::ivec3(x, y, z));
}

bool Chunk::isFaceVisible(glm::ivec3 block_pos) const
{
	int x = block_pos.x, y = block_pos.y, z = block_pos.z;
	// out of bounds check for example: x - 1 = -1 < 0, x + 1 = 16 > 15
	if (x < 0 || y < 0 || z < 0 || x >= m_lod.block_amount || y >= m_lod.block_amount || z >= m_lod.block_amount)
	{
		/* world_pos is incorrectly calculated */

		//glm::vec3 world_pos{ m_world_pos.x + x, m_world_pos.y + y, m_world_pos.z + z };
		//glm::ivec3 neighbour_chunk_pos = floor(world_pos / static_cast<float>(CHUNK_SIZE));
		//int neighbour_lod_block_size = m_chunk_manager->getChunksMap().at(neighbour_chunk_pos).getLevelOfDetail().block_size;
		//if (m_lod.block_size != neighbour_lod_block_size) // Don't try to reason about not matching LODs
		//	return false;

		return false;
		//return m_chunk_manager->getChunkBlockId(world_pos) != block_id::AIR;
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

	Vertex vertex;
	vertex.packed_vertex = packVertex(x, y, z, texture_id, face_id);

	Face face;
	face.packed_face = packFace(x, y, z, texture_id, face_id);

	for (GLubyte vertex_id = 0; vertex_id < Block::VERTICES_PER_FACE; vertex_id++)
	{
		m_mesh.addVertex(vertex);
	}
	m_faces.push_back(face);
	m_added_faces++;
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
