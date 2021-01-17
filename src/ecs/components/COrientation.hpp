#pragma once

#include <glm/glm.hpp>

struct COrientation {
	glm::vec3 orientation;
	float amount;
	
	COrientation(glm::vec3 orientation, float amount)
		: orientation(orientation), amount(amount)
	{
	}
	
};
