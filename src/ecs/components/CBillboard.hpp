#pragma once

#include <glm/glm.hpp>

struct CBillboard {
	glm::vec2 size;
	
	CBillboard(glm::vec2 size)
		:  size(size)
	{
	}
	
};
