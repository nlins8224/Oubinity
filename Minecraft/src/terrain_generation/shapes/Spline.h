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
	SplineRange getLinearRange(std::array<glm::vec2, 4> points);
	int translateToKey(double value);
	double translateToValue(int value);
	int getKeyTranslateValue();
private:	
	// spline map keys maps [0, 1000] -> [0.000, 1.000], E.G. 567 -> 0.567
	int m_key_translate_value{ 1000 };
	float m_start_range{ 0.0f };
	float m_end_range{ 1.0f };
};