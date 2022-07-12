#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtx/integer.hpp>

struct COrientedTexture {
	int directions;
	float angle;
	float camera_angle;

	int get_sprite_offset() const {
		const float half_direction_angle = ((glm::pi<float>() * 2.f) / float(directions)) * 0.5f;

		float rad = angle;
		rad -= (camera_angle - half_direction_angle);

		// convert to unsigned angle
		while (rad < 0.f)
			rad += glm::pi<float>() * 2.f;

		int direction = glm::floor((rad / (glm::pi<float>() * 2.f)) * float(directions));
		return glm::mod(direction, directions);
	}
};
