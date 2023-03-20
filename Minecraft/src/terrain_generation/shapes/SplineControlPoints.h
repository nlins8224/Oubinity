#pragma once
#include <array>
#include <glm/glm.hpp>
#include "../NoiseMapTypes.h"
#include "Spline.h"

namespace SplineControlPoints
{
	extern std::array<glm::vec2, 4> flat;
	extern std::array<glm::vec2, 4> mountain;
	extern std::array<glm::vec2, 4> base;
}
