#pragma once
#include <string>
namespace Block {

	// later this could be moved to .json or .ini file

	enum block_id
	{
		NONE = -1,
		AIR,
		DIRT,
		COBBLESTONE,
		SAND,
		PLANKS
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

	inline Block getBlockType(block_id block_id)
	{
		switch (block_id)
		{
		case AIR:         return Air;
		case DIRT:        return Dirt;
		case COBBLESTONE: return Cobblestone;
		case SAND:        return Sand;
		case PLANKS:      return Planks;
		default:		  return None;
		}
	}

}

