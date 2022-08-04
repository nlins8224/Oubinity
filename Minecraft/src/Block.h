#pragma once
#include <string>
#include <map>
namespace Block {

	enum block_id
	{
		AIR,
		DIRT,
		COBBLESTONE,
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

	static const std::map<int, Block> TYPES_MAP
	{
		{AIR, Air},
		{DIRT, Dirt},
		{COBBLESTONE, Cobblestone}
	};

	inline Block getBlockType(int block_id)
	{
		return TYPES_MAP.at(block_id);
	}

	// private hashmap could be here and function getBlockType() which would return block of given type
	// for example getBlockType(block_id::DIRT) would reurn Block Dirt
	// later this could be moved to .json or .ini file

}

