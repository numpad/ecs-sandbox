#pragma once

#include <glm/glm.hpp>

struct CVelocity {
	glm::vec3 vel, acc;
	float maxvel = 0.075f;

	CVelocity(glm::vec3 v) : vel(v), acc(0.0f) {
	}
	CVelocity(glm::vec3 v, glm::vec3 a) : vel{v}, acc{a} {
	}
	CVelocity(float x, float y, float z) : vel(x, y, z), acc(0.0f) {
	}
	CVelocity() : CVelocity(0.0f, 0.0f, 0.0f) {
	}
};
