#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtx/integer.hpp>

struct COrientedTexture {
	int directions;
	float angle;
	float camera_angle;

	int get_sprite_offset() const;
};
