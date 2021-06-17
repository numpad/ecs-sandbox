#pragma once

#include <glm/glm.hpp>

struct CTextureRegion {
	glm::vec4 region;
	bool is_flipped = false;

	CTextureRegion(glm::vec4 value)
	 : region{value}, is_flipped{false}
	{
	}

	CTextureRegion(float x, float y, float w, float h, int texw, int texh, bool flip_x = false) {
		is_flipped = flip_x;		
		float ox = x / float(texw);
		float oy = (float(texh) - y - h) / float(texh);
		float ow = w / float(texw);
		float oh = h / float(texh);
		
		if (flip_x) region = glm::vec4(ox + ow, oy, -ow, oh);
		else        region = glm::vec4(ox     , oy,  ow, oh);
	}
};
