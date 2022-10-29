#pragma once
#include <string>
namespace Block {

	// later this could be moved to .json or .ini file

	enum block_id
	{
		AIR,
		DIRT,
		COBBLESTONE,
		SAND,
		AMOUNT
	};

	struct Block
	{
		block_id id;
		bool transparent{ false };
		std::string texture{ "" };
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

	inline Block getBlockType(block_id block_id)
	{
		switch (block_id)
		{
		case AIR:         return Air;
		case DIRT:        return Dirt;
		case COBBLESTONE: return Cobblestone;
		case SAND:        return Sand;
		default:		  return Air; // "not_found.png" could be here
		}
	}

}

