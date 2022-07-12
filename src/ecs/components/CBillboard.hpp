#pragma once

#include <glm/glm.hpp>
#include <GL/gl3w.h>
#include <Assets/Texture.hpp>
#include <Util/Random.hpp>

struct CBillboard {
	glm::vec2 size;
	Texture* texture;

	CBillboard(Texture* texture, glm::vec2 size) : size(size), texture(texture) {
	}
};
