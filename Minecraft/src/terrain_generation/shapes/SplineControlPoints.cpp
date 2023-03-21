#include "SplineControlPoints.h"

std::vector<glm::vec2> SplineControlPoints::flat{ {{0.0f, 30.0f}, {0.15f, 95.0f}, {0.35f, 85.0f}, {1.0f, 65.0f}} };
std::vector<glm::vec2> SplineControlPoints::mountain{ {{-1.0f, 50.0f}, {0.3f, 100.0f}, {0.4f, 150.0f}, {1.0f, 150.0f}} };
std::vector<glm::vec2> SplineControlPoints::base{ {{0.0f, 20.0f}, {0.5f, 65.0}, {0.6f, 140.0f}, {1.0f, 170.0f}} };
std::vector<glm::vec2> SplineControlPoints::test{ {{0.0f, 20.0f}, {0.20f, 90.0f}, {0.325f, 55.0f}, {0.4f, 65.0}, {0.75f, 160.0f}, {1.0f, 120.0f}} };

