#include "sgl/sgl_renderbuffer.hpp"

sgl::renderbuffer::renderbuffer() {
	glGenRenderbuffers(1, &m_handle);
}

sgl::renderbuffer::~renderbuffer() {
	glDeleteRenderbuffers(1, &m_handle);
}

void sgl::renderbuffer::create(int width, int height, sgl::renderbuffer::internalformat format) {
	if (m_width == width && m_height == height && m_format == static_cast<GLenum>(format)) return;
	
	m_width = width;
	m_height = height;
	m_format = static_cast<GLenum>(format);
	
	bind();
	glRenderbufferStorage(GL_RENDERBUFFER, m_format, m_width, m_height);
	unbind();
}

void sgl::renderbuffer::bind() {
	glBindRenderbuffer(GL_RENDERBUFFER, m_handle);
}

void sgl::renderbuffer::unbind() {
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

sgl::renderbuffer::operator GLuint() const {
	return m_handle;
}
