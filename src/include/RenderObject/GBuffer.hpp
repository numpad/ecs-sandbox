#pragma once

#include <sgl/sgl_framebuffer.hpp>
#include <sgl/sgl_texture.hpp>
#include <sgl/sgl_renderbuffer.hpp>
#include <sgl/sgl_attachment.hpp>
#include <sgl/sgl_shader.hpp>

class GBuffer {
public:

	bool initialize(int width, int height);
	void destroy();

	void resize(int width, int height);

	void bind();
	void unbind();

	void bind_textures(sgl::shader &shader);


public: // TODO: should be private
	// attachments
	sgl::texture *m_color;
	sgl::texture *m_position;
	sgl::texture *m_normal;
	sgl::texture *m_depth;
	sgl::renderbuffer *m_depth_stencil;

	// framebuffer
	sgl::framebuffer *m_fbo;
private:
	int m_width, m_height;

};
