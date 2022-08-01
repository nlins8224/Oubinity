#pragma once
#include <string>
namespace Block {

	enum block_id
	{
		AIR,
		DIRT,
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

	// private hashmap could be here and function getBlockType() which would return block of given type
	// for example getBlockType(block_id::DIRT) would reurn Block Dirt
	// later this could be moved to .json or .ini file

}

