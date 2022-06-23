#pragma once

#include <glm/glm.hpp>

struct CPosition {
	glm::vec3 pos;

	CPosition() : pos(0.f, 0.f, 0.f) {
	}

	CPosition(float x, float y, float z) : pos(x, y, z) {
	}

	CPosition(glm::vec3 p) : pos(p) {
	}
};
