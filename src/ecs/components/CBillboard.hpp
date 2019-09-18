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
	
	CBillboard(const Texture *texture, glm::vec2 size, glm::vec3 color = glm::vec3(1.0f))
		:  size(size), color(color), texture(texture), texture_offset(0.0f, 0.0f, 1.0f, 1.0f)
	{
		float x = floor(Random(0.0f, 6.0f)());
		float y = 10.0f + floor(Random(0.0f, 5.0f)());
		setSubRect(x * 16.0f, y * 16.0f, 16.0f, 16.0f, 256, 256);
	}
	
	void setSubRect(float x, float y, float w, float h, int texw, int texh, bool flip_x = false) {
		sr_pos = glm::vec2(x, y);
		sr_size = glm::vec2(w, h);
		sr_texsize = glm::ivec2(texw, texh);
		sr_flip = flip_x;
		
		float ox = x / float(texw);
		float oy = y / float(texh);
		float ow = w / float(texw);
		float oh = h / float(texh);
		
		if (flip_x) this->texture_offset = glm::vec4(ox + ow, 1.0f - oy, -ow, oh);
		else this->texture_offset = glm::vec4(ox, 1.0f - oy, ow, oh);
	}
	void resetSubRect() {
		texture_offset = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
	}
	
	bool isFlipped() { return sr_flip; }
	void setFlipped(bool flip) {
		this->setSubRect(sr_pos.x, sr_pos.y, sr_size.x, sr_size.y,
			sr_texsize.x, sr_texsize.y, flip);
	}
	
private:
	// subrect:
	glm::vec2 sr_pos, sr_size;
	glm::ivec2 sr_texsize;
	bool sr_flip = false;
	
};
