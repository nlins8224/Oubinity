#include "AABox.h"

AABox::AABox(glm::vec3 world_pos, glm::vec3 box_size)
	: 
	m_corner_pos{world_pos},
	m_box_size{box_size}
{
}

glm::vec3 AABox::getVertexPositiveTo(const glm::vec3& normal) const
{
	glm::vec3 res = m_corner_pos;

	if (normal.x > 0)
		res.x += m_box_size.x;
	if (normal.y > 0)
		res.y += m_box_size.y;
	if (normal.z > 0)
		res.z += m_box_size.z;

	return res;
}
