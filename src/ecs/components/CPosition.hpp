#pragma once

#include <glm/glm.hpp>

struct CPosition {
	glm::vec3 pos;
	
	CPosition(float x, float y, float z)
		: pos(x, y, z)
	{
	}
	
	CPosition(glm::vec3 p)
		: pos(p)
	{
	}
};
