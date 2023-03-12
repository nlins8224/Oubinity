#pragma once
#include <unordered_map>
#include "glm/glm.hpp";
#include "../../third_party/bezier.h"
#include "../NoiseMapTypes.h"

class Spline
{
public:
	Spline();
	SplineRange getBezierRange(std::array<glm::vec2, 4> points);
	int translateToKey(double value);
	double translateToValue(int value);
	int getKeyTranslateValue();
private:	
	// spline map keys maps [-1000, 1000] -> [-1.000, 1.000]
	// E.G. 567 -> 0.567, -12 -> -0.012 
	int m_key_translate_value{ 1000 };
};