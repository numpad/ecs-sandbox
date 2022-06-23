#pragma once

#include <glm/glm.hpp>
#include <Assets/Texture.hpp>

struct CDecal {
	glm::vec3 size;

	Texture* texture;
	glm::vec4 texture_offset;
};
