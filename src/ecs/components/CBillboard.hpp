#pragma once

#include <glm/glm.hpp>
#include <GL/gl3w.h>
#include <Assets/Texture.hpp>
#include <Util/Random.hpp>

struct CBillboard {
	glm::vec2 size;
	
	Texture *texture;
	glm::vec4 texture_offset; // xy = offset , zw = scale
	
	CBillboard(Texture *texture, glm::vec2 size)
		:  size(size), texture(texture)
	{
		
	}
	
	void setSubRect(float x, float y, float w, float h, int texw, int texh, bool flip_x = false) {
		sr_flip = flip_x;		
		float ox = x / float(texw);
		float oy = (float(texh) - y - h) / float(texh);
		float ow = w / float(texw);
		float oh = h / float(texh);
		
		if (flip_x) texture_offset = glm::vec4(ox + ow, oy, -ow, oh);
		else        texture_offset = glm::vec4(ox     , oy,  ow, oh);
	}
	
	// flip 
	bool isFlipped() { return sr_flip; }
	void setFlipped(bool flip) {
		if (flip != sr_flip) {
			texture_offset.x += texture_offset.z;
			texture_offset.z *= -1.0f;
			sr_flip = flip;
		}
	}
	
private:
	// subrect flipping
	bool sr_flip = false;
	
	
};
