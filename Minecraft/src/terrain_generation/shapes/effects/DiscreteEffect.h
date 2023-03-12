#pragma once
#include "Effect.h"

class DiscreteEffect : public Effect
{
public:
	void addEffect(HeightMap& height_map) override;
private:
	double truncate(double value);
};