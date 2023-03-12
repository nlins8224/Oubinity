#include "Spline.h"
#include <iostream>

Spline::Spline()
{
	m_key_translate_value = 1000;
}

/* Translates [-1.000, 1.000] -> [-1000, 1000] */
int Spline::translateToKey(double value)
{
	return static_cast<int>(value * m_key_translate_value);
}

/* Translates [-1000, 1000] -> [-1.000, 1.000] */
double Spline::translateToValue(int value)
{
	return static_cast<double>(value) / m_key_translate_value;
}


int Spline::getKeyTranslateValue()
{
	return m_key_translate_value;
}

SplineRange Spline::getBezierRange(std::array<glm::vec2, 4> points)
{
	Bezier::Bezier<3> cubicBezier({ 
		{points[0].x, points[0].y}, 
		{points[1].x, points[1].y}, 
		{points[2].x, points[2].y}, 
		{points[3].x, points[3].y} });

	SplineRange spline_map;
	double t = 1.0f / static_cast<double>(m_key_translate_value);
	const int y = 1;
	for (double k = -1.0f; k < 1.0f; k += t)
	{
		double height = cubicBezier.valueAt(k, y);
		int key = translateToKey(k);
		spline_map.emplace(key, height);
	}

	return spline_map;
}