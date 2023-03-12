#pragma once
#include "../../NoiseMapTypes.h"

class Effect
{
public:
	virtual void addEffect(HeightMap& height_map) = 0;
};