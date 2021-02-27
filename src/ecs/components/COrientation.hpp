#pragma once

#include <glm/glm.hpp>

struct COrientation {
	glm::vec3 orientation;
	float amount;
	
	COrientation()
		: orientation(glm::vec3(0.f, -1.f, 0.f)), amount(1.f)
	{
	}

	COrientation(glm::vec3 orientation, float amount)
		: orientation(orientation), amount(amount)
	{
	}
	
};
