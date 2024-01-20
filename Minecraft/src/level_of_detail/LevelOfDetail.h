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
		.level{ 0 },
		.block_amount{ CHUNK_SIZE },
		.block_size{ 1.0f },
		.draw_distance{ 0 },
		.divide_factor{ 1 }
	};

	static const LevelOfDetail One
	{
		.level{ 1 },
		.block_amount{ CHUNK_SIZE / 2 },
		.block_size{ 2.0f },
		.draw_distance{ 16 },
		.divide_factor{ 2 }
	};

	static const LevelOfDetail Two
	{
		.level{ 2 },
		.block_amount{ CHUNK_SIZE / 4 },
		.block_size{ 4.0f },
		.draw_distance{ 32 },
		.divide_factor{ 4 }
	};

	static const LevelOfDetail Three
	{
		.level{ 3 },
		.block_amount{ CHUNK_SIZE / 8 },
		.block_size{ 8.0f },
		.draw_distance{ 64 },
		.divide_factor{ 8 }
	};

	static const LevelOfDetail Four
	{
		.level{ 4 },
		.block_amount{ CHUNK_SIZE / 16 },
		.block_size{ 16.0f },
		.draw_distance{ 128 },
		.divide_factor{ 16 }
	};

	static const LevelOfDetail Five
	{
		.level{ 5 },
		.block_amount{ CHUNK_SIZE / 32 },
		.block_size{ 32.0f },
		.draw_distance{ 256 },
		.divide_factor{ 32 }
	};

	static const std::vector<LevelOfDetail> Lods
	{
		{ Zero },
		{ One },
		{ Two },
		{ Three },
		{ Four },
		{ Five }
	};

	static uint16_t distanceToCameraInChunks(Camera& camera, glm::ivec3 chunk_pos)
	{
		glm::ivec3 camera_pos = camera.getCameraPos() / static_cast<float>(CHUNK_SIZE);

		/* 
		   draw_distance |AC| segment consists of N chunks, N is even
		   Camera is located in chunk B, in half of |AC| segment
		   |AC| segment is divided to two segments: |AB| and |B'C|, where B' is chunk next to chunk B
		   |AB| segment contains chunk B where camera is
		   |B'C| segment does not contain chunk where camera is
		   If distance being calculated is between chunks B and A, we add 1 to take into account chunk B

		   Example 1:
		   AXXBXXXC
		   C - B = XXXC
		   B - A = AXX
		   Include B to even out distances: AXXB

		   Example 2:
		   Point A: (-3, 0)
		   Point B: (0, 0) <- camera position
		   Point C: (0, 4)

		   |0 - (-3)| = 3, the distance to camera is correct, but we should include camera chunk and return 4 to calculate LOD correctly
		*/
		bool is_x_in_camera_segment = chunk_pos.x < camera_pos.x;
		bool is_z_in_camera_segment = chunk_pos.z < camera_pos.z;

		uint16_t distance_x = std::abs(camera_pos.x - chunk_pos.x);
		uint16_t distance_z = std::abs(camera_pos.z - chunk_pos.z);

		distance_x += is_x_in_camera_segment;
		distance_z += is_z_in_camera_segment;

		return std::max(distance_x, distance_z);
	}

	static LevelOfDetail chooseLevelOfDetail(Camera& camera, glm::ivec3 chunk_pos)
	{
		uint16_t draw_distance = distanceToCameraInChunks(camera, chunk_pos);
		// multiply by 2, because distance to camera is exactly in half of the segment
		draw_distance *= 2;

		if (draw_distance <= One.draw_distance)
			return Zero;
		if (draw_distance <= Two.draw_distance)
			return One;
		if (draw_distance <= Three.draw_distance)
			return Two;
		if (draw_distance <= Four.draw_distance)
			return Three;
		if (draw_distance <= Five.draw_distance)
			return Four;

		return Five;
	}
}

