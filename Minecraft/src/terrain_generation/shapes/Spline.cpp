#include "Spline.h"
#include <iostream>

Spline::Spline()
{
	m_key_translate_value = 1000;
}

/* Translates [0.000, 1.000] -> [0, 1000] */
int Spline::translateToKey(double value)
{
	return static_cast<int>(value * m_key_translate_value);
}

/* Translates [0, 1000] -> [0.000, 1.000] */
double Spline::translateToValue(int value)
{
	return static_cast<double>(value) / m_key_translate_value;
}


int Spline::getKeyTranslateValue()
{
	return m_key_translate_value;
}

SplineRange Spline::getBezierRange(std::vector<glm::vec2> points)
{
	Bezier::Bezier<3> cubicBezier({ 
		{points[0].x, points[0].y}, 
		{points[1].x, points[1].y}, 
		{points[2].x, points[2].y}, 
		{points[3].x, points[3].y} });

	SplineRange spline_map;
	double t = 1.0f / static_cast<double>(m_key_translate_value);
	for (float k = m_start_range; k < m_end_range; k += t)
	{
		float height = cubicBezier.valueAt(k).y;
		int key = translateToKey(k);
		//std::cout << k << " " << key << " " << height << std::endl;
		spline_map.emplace(key, height);
	}

	return spline_map;
}

SplineRange Spline::getLinearRange(std::vector<glm::vec2> points)
{
	SplineRange spline_map;
	double t = 1.0f / static_cast<double>(m_key_translate_value);
	for (int i = 0; i < points.size() - 1; i++)
	{
		glm::vec2 start_point = points[i];
		glm::vec2 end_point = points[i + 1];

		float range = (end_point.x - start_point.x) * m_key_translate_value;
		float multiplier = m_key_translate_value / range;
		float step = t * multiplier;
		int step_iter = 0;

		for (float k = start_point.x; k < end_point.x; k += t)
		{
			int key = translateToKey(k);
			float height = std::lerp(start_point.y, end_point.y, step_iter * step);
			//std::cout << k << " " << key << " " << height << std::endl;
			spline_map.emplace(key, height);
			step_iter++;
		}
	}

	return spline_map;
}
