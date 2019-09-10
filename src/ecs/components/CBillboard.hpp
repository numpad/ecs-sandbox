#pragma once

#include <glm/glm.hpp>
#include <GL/gl3w.h>
#include <Assets/Texture.hpp>
#include <Util/Random.hpp>

struct CBillboard {
	glm::vec2 size;
	glm::vec3 color;
	
	const Texture *texture;
	glm::vec4 texture_offset; // xy = offset , zw = scale
	
	CBillboard(glm::vec2 size, glm::vec3 color = glm::vec3(1.0f))
		:  size(size), color(color), texture_offset(0.0f, 0.0f, 1.0f, 1.0f)
	{
		float x = floor(Random(0.0f, 5.0f)());
		float y = 11;
		setSubRect(x * 16.0f, y * 16.0f, 16.0f, 16.0f, 256, 256);
	}
	
	void setSubRect(float x, float y, float w, float h, int texw, int texh, bool flip_x = false) {
		float ox = x / float(texw);
		float oy = y / float(texh);
		float ow = w / float(texw);
		float oh = h / float(texh);
		
		if (flip_x) this->texture_offset = glm::vec4(ox + ow, 1.0f - oy, -ow, oh);
		else this->texture_offset = glm::vec4(ox, 1.0f - oy, ow, oh);
	}
};
