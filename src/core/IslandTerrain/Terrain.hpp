#pragma once

#include <glm/glm.hpp>

struct Terrain {
	glm::ivec3 size;
	float surface = 0.0f;
	float* values = nullptr;
};


