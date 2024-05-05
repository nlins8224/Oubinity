#pragma once
#include <string>
#include <set>

namespace Block {

	enum block_id : uint8_t
	{
		AIR = 0,
		DIRT = 1,
		COBBLESTONE = 2,
		SAND = 3,
		PLANKS = 4,
		STONE = 5,
		GRASS = 6,
		WATER = 7,
		OAK_LOG = 8,
		OAK_LEAVES = 9,
		AMOUNT = 10,
		NONE = 255
	};

	struct Block
	{
		block_id id;
		bool transparent{ false };
		std::string texture{ "" };
	};

	static const Block None
	{
		.id{ NONE },
		.texture{ "not_found" }
	};

	static const Block Air
	{
		.id{ AIR },
		.transparent{ true }
	};

	static const Block Dirt
	{
		.id{ DIRT },
		.texture{ "dirt" }
	};

	static const Block Cobblestone
	{
		.id{ COBBLESTONE },
		.texture{ "cobblestone" }
	};

	static const Block Sand
	{
		.id{ SAND },
		.texture{ "sand" }
	};

	static const Block Planks
	{
		.id{ PLANKS },
		.texture{ "planks" }
	};

	static const Block Stone
	{
		.id{ STONE },
		.texture{ "stone" }
	};

	static const Block Grass
	{
		.id{ GRASS },
		.texture{ "grass" }
	};

	static const Block Water
	{
		.id{ WATER },
		.texture{ "water" }
	};

	static const Block Oak_Log
	{
		.id{ OAK_LOG },
		.texture{ "oak_log" }
	};
	
	static const Block Oak_Leaves
	{
		.id { OAK_LEAVES },
		.texture{ "oak_leaves" }
	};

	inline Block getBlockType(block_id block_id)
	{
		switch (block_id)
		{
		case AIR:         return Air;
		case DIRT:        return Dirt;
		case COBBLESTONE: return Cobblestone;
		case SAND:        return Sand;
		case PLANKS:      return Planks;
		case STONE:		  return Stone;
		case GRASS:		  return Grass;
		case WATER:		  return Water;
		case OAK_LOG:	  return Oak_Log;
		case OAK_LEAVES:  return Oak_Leaves;
		default:		  return Air;
		}
	}

	struct Pixel_RGBA
	{
		uint8_t r;
		uint8_t g;
		uint8_t b;
		uint8_t a;
	};

	inline Pixel_RGBA getBlockColor(block_id block_id)
	{
		switch (block_id)
		{
		case DIRT:        return { 161, 124, 70, 0 };
		case SAND:        return { 190, 190, 128, 0 };
		case STONE:		  return { 128, 128, 128, 0 };
		case GRASS:		  return { 67, 129, 64, 0 };
		default:		  return { 0, 0, 0, 0 };
		}
	}

	static const std::set<block_id> decoration_set{ OAK_LOG, OAK_LEAVES };
}

