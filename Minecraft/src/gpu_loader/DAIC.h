#pragma once

struct DAIC
{
	DAIC() = default;
	DAIC(unsigned int c, unsigned int ic, unsigned int f, unsigned int bi)
	{
		count = c;
		instance_count = ic;
		first = f;
		base_instance = bi;
	}

	unsigned int  count;
	unsigned int  instance_count;
	unsigned int  first;
	unsigned int  base_instance;
};