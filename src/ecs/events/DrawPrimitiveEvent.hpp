#pragma once

#include <vector>
#include <glm/glm.hpp>

using namespace glm;

struct DrawPrimitiveEvent {
	DrawPrimitiveEvent(vec3 begin, vec3 end) : vertices{begin, end} {}
	
	std::vector<vec3> vertices;
};
