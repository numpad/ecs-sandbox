#pragma once

#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <sgl/sgl_framebuffer.hpp>
#include <sgl/sgl_texture.hpp>
#include "Util/Font.hpp"

class Terminal {
public:

	Terminal();
	~Terminal();

	void update();

	GLuint getTexture() const { return (GLuint)(*m_texture); }

private:
	int m_width = 512;
	int m_height = 512;
	sgl::framebuffer *m_fbo;
	sgl::texture *m_texture;

	Font *m_font;
	std::wstring m_text;

};
