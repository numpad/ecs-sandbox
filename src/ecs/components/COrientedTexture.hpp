#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

struct COrientedTexture {
	int directions;
	float angle;

	int get_direction() const {
		float rad = angle;
		int deg = int(glm::degrees(angle));
		if (glm::abs(deg) == 45) {
			rad += glm::sign(deg) * 0.4f;
		}

		return int((rad / (glm::pi<float>() * 2.f)) * directions);
	}
};
