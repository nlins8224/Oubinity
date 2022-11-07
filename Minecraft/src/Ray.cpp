#include "Ray.h"

double Ray::getDistance()
{
	return m_distance_travelled;
}

bool Ray::step(std::function<void(glm::vec3, glm::vec3)> hit_callback)
{
	int sign[3] = { 1, 1, 1 };
	for (int i : {0, 1, 2})
		if (m_direction[i] < 0)
			sign[i] = -1;


	glm::vec3 abs_direction = glm::abs(m_direction);
	glm::vec3 abs_local_pos = glm::abs(m_current_pos - m_world_block_pos);

	double lx = abs_local_pos.x;
	double ly = abs_local_pos.y;
	double lz = abs_local_pos.z;

	double dx = abs_direction.x;
	double dy = abs_direction.y;
	double dz = abs_direction.z;

	// Distance from center of a block to a face in a straight line
	const double FACE_OFFSET = 0.5;

	// Calculate intersections
	if (dx != 0.0)
	{
		double x = FACE_OFFSET;
		double y = (FACE_OFFSET - lx) / dx * dy + ly;
		double z = (FACE_OFFSET - lx) / dx * dz + lz;

		if (y >= -FACE_OFFSET && y <= FACE_OFFSET &&
			z >= -FACE_OFFSET && z <= FACE_OFFSET)
		{
			double distance_squared = (x - lx) * (x - lx) + (y - ly) * (y - ly) + (z - lz) * (z - lz);
			double distance = sqrt(distance_squared);
			return check(hit_callback, distance, m_world_block_pos, m_world_block_pos + glm::vec3(sign[0], 0.0, 0.0));
		}
	}

	if (dy != 0.0)
	{
		double x = (FACE_OFFSET - ly) / dy * dx + lx;
		double y = FACE_OFFSET;
		double z = (FACE_OFFSET - ly) / dy * dz + lz;

		if (x >= -FACE_OFFSET && x <= FACE_OFFSET &&
			z >= -FACE_OFFSET && z <= FACE_OFFSET)
		{
			double distance_squared = (x - lx) * (x - lx) + (y - ly) * (y - ly) + (z - lz) * (z - lz);
			double distance = sqrt(distance_squared);
			return check(hit_callback, distance, m_world_block_pos, m_world_block_pos + glm::vec3(0.0, sign[1], 0.0));
		}
	}

	if (dz != 0.0)
	{
		double x = (FACE_OFFSET - lz) / dz * dx + lx;
		double y = (FACE_OFFSET - lz) / dz * dy + ly;
		double z = FACE_OFFSET;

		if (x >= -FACE_OFFSET && x <= FACE_OFFSET &&
			y >= -FACE_OFFSET && y <= FACE_OFFSET)
		{
			double distance_squared = (x - lx) * (x - lx) + (y - ly) * (y - ly) + (z - lz) * (z - lz);
			double distance = sqrt(distance_squared);
			return check(hit_callback, distance, m_world_block_pos, m_world_block_pos + glm::vec3(0.0, 0.0, sign[2]));
		}
	}

	std::cout << "Ray::step, Miss!" << std::endl;
	return true;
}

bool Ray::check(std::function<void(glm::vec3, glm::vec3)> hit_callback, double distance, glm::vec3 current_block, glm::vec3 next_block)
{
	if (m_world.getChunkBlockId(next_block) != Block::AIR)
	{
		hit_callback(current_block, next_block);
		return true;
	}
	
	glm::vec3 pos_change{ m_direction.x * distance, m_direction.y * distance, m_direction.z * distance };
	m_current_pos += pos_change;
	m_world_block_pos = next_block;
	m_distance_travelled += distance;
	
	return false;
}