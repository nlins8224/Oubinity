#pragma once
#include <unordered_map>
#include "../chunk/ChunkSize.h"
#include "../chunk/Chunk.h"
#include "../Camera.h"

namespace LevelOfDetail {

	struct LevelOfDetail
	{
		int level;
		int block_amount;
		float block_size;
		int draw_distance; // how many chunks away from the camera this LOD level should start being active
		int divide_factor;
	};

	static const LevelOfDetail Zero
	{
		.level{ 1 },
		.block_amount{ CHUNK_SIZE },
		.block_size{ 1.0f },
		.draw_distance{ 0 },
		.divide_factor{ 1 }
	};

	static const LevelOfDetail One
	{
		.level{ 2 },
		.block_amount{ CHUNK_SIZE / 2 },
		.block_size{ 2.0f },
		.draw_distance{ 8 },
		.divide_factor{ 2 }
	};

	static const LevelOfDetail Two
	{
		.level{ 3 },
		.block_amount{ CHUNK_SIZE / 4 },
		.block_size{ 4.0f },
		.draw_distance{ 16 },
		.divide_factor{ 4 }
	};

	static const LevelOfDetail Three
	{
		.level{ 4 },
		.block_amount{ CHUNK_SIZE / 8 },
		.block_size{ 8.0f },
		.draw_distance{ 32 },
		.divide_factor{ 8 }
	};

	static const LevelOfDetail Four
	{
		.level{ 5 },
		.block_amount{ CHUNK_SIZE / 16 },
		.block_size{ 16.0f },
		.draw_distance{ 64 },
		.divide_factor{ 16 }
	};

	static const LevelOfDetail Five
	{
		.level{ 6 },
		.block_amount{ CHUNK_SIZE / 32 },
		.block_size{ 32.0f },
		.draw_distance{ 128 },
		.divide_factor{ 32 }
	};

	static uint16_t distanceToCameraInChunks(Camera& camera, glm::ivec3 chunk_pos)
	{
		glm::ivec3 camera_pos = camera.getCameraPos() / static_cast<float>(CHUNK_SIZE);

		uint16_t distance_x = std::abs(camera_pos.x - chunk_pos.x);
		uint16_t distance_z = std::abs(camera_pos.z - chunk_pos.z);

		return std::max(distance_x, distance_z);
	}

	static LevelOfDetail chooseLevelOfDetail(Camera& camera, glm::ivec3 chunk_pos)
	{
		uint16_t draw_distance = distanceToCameraInChunks(camera, chunk_pos);

		if (draw_distance < One.draw_distance)
			return Zero;
		if (draw_distance < Two.draw_distance)
			return One;
		if (draw_distance < Three.draw_distance)
			return Two;
		if (draw_distance < Four.draw_distance)
			return Three;
		if (draw_distance < Five.draw_distance)
			return Four;

		return Five;
	}
}

