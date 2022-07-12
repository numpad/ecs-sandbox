#pragma once

#include <glm/glm.hpp>

struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texcoords;

	Vertex(glm::vec3 pos = glm::vec3(0.0f), glm::vec3 normal = glm::vec3(0.0f), glm::vec2 uv = glm::vec2(0.0f));
};
