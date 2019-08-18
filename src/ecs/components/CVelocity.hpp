#pragma once

#include <glm/glm.hpp>

struct CVelocity {
	glm::vec3 vel;
	
	CVelocity(glm::vec3 v) : vel(v) {}
	CVelocity(float x, float y, float z) : vel(x, y, z) {}
	CVelocity() : CVelocity(0.0f, 0.0f, 0.0f) {}
};
