#pragma once

#include <glm/glm.hpp>

struct CBillboard {
	glm::vec2 size;
	glm::vec3 color;

	CBillboard(glm::vec2 size, glm::vec3 color = glm::vec3(1.0f))
		:  size(size), color(color)
	{
	}
};
