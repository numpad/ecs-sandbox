#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

struct COrientedTexture {
	int directions;
	float angle;

	int get_direction() const {
		return int((angle / (glm::pi<float>() * 2.f)) * directions);
	}
};
