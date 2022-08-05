#pragma once
#include <string>
#include <map>
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

	static const std::map<int, Block> TYPES_MAP
	{
		{AIR, Air},
		{DIRT, Dirt},
		{COBBLESTONE, Cobblestone},
		{SAND, Sand}
	};

	inline Block getBlockType(int block_id)
	{
		return TYPES_MAP.at(block_id);
	}


}

