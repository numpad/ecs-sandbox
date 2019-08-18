#pragma once

#include <glm/glm.hpp>

struct CBillboard {
	glm::vec3 color;
	glm::vec2 size;
	
	CBillboard(glm::vec3 color, glm::vec2 size)
		: color(color), size(size)
	{
	}
	
};
