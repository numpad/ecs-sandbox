#include "RenderObject/GBuffer.hpp"

bool GBuffer::initialize(int width, int height) {
	m_width = width;
	m_height = height;

	// initialize buffers
	m_fbo = new sgl::framebuffer();

	m_color = new sgl::texture();
	m_position = new sgl::texture();
	m_normal = new sgl::texture();
	m_depth = new sgl::texture();
	m_depth_stencil = new sgl::renderbuffer();

	// create buffers
	m_color->load(   width, height, sgl::texture::internalformat::rgba, nullptr, sgl::texture::format::rgba, sgl::texture::datatype::u8);
	m_position->load(width, height, sgl::texture::internalformat::rgba16f); // TODO: remove, instead reconstruct position from depth buffer to save vram
	m_normal->load(  width, height, sgl::texture::internalformat::rgba16f); // TODO: GL_RGB10_A2 should give enough accuracy.
	m_depth->load(   width, height, sgl::texture::internalformat::rgba16f);
	m_depth_stencil->create(width, height, sgl::renderbuffer::internalformat::depth24_stencil8);
	// configure
	// m_color.set_filter()??
	m_position->set_filter(sgl::texture::filter::nearest);
	m_normal->set_filter(sgl::texture::filter::nearest);
	m_depth->set_filter(sgl::texture::filter::nearest);
	
	// attach
	m_fbo->attach(*m_color,    sgl::attachment::color(0));
	m_fbo->attach(*m_position, sgl::attachment::color(1));
	m_fbo->attach(*m_normal,   sgl::attachment::color(2));
	m_fbo->attach(*m_depth,    sgl::attachment::color(3));
	m_fbo->attach(*m_depth_stencil, sgl::attachment::depth_stencil());
	m_fbo->targets();

	return true;
}

void GBuffer::destroy() {
	delete m_fbo;
	delete m_color;
	delete m_position;
	delete m_normal;
	delete m_depth;
	delete m_depth_stencil;
}

void GBuffer::resize(int width, int height) {
	// only resize when needed
	if (width == m_width && height == m_height) return;

	m_width = width;
	m_height = height;

	m_color->resize(width, height);
	m_position->resize(width, height);
	m_normal->resize(width, height);
	m_depth->resize(width, height);
	m_depth_stencil->create(width, height, sgl::renderbuffer::internalformat::depth24_stencil8);
}

void GBuffer::bind() {
	m_fbo->bind();
}

void GBuffer::unbind() {
	m_fbo->unbind();
}

void GBuffer::bind_textures() {
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, *m_color);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, *m_position);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, *m_normal);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, *m_depth);
}
