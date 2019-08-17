#pragma once

#include <glm/glm.hpp>

struct CBillboard {
	glm::vec3 color;
	
	CBillboard(float r, float g, float b)
		: color(r, g, b)
	{
	}
	
};
