#pragma once
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
	};

	static const Block Air
	{
		.id{ AIR },
		.transparent{ true }
	};

	static const Block Dirt
	{
		.id{ DIRT }
	};

}

