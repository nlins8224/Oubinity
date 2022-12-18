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
		PLANKS,
		STONE,
		GRASS,
		AMOUNT
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
		default:		  return None;
		}
	}

}

