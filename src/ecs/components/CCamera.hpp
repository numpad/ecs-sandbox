#pragma once

#include <glm/glm.hpp>

struct CCamera {
	float near_distance;
	float far_distance;

	glm::mat4 projection;
	glm::mat4 view;

};
