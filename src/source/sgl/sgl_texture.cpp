#include "sgl/sgl_texture.hpp"

sgl::texture::texture() {
	glGenTextures(1, &m_handle);
}

sgl::texture::~texture() {
	glDeleteTextures(1, &m_handle);
}

void sgl::texture::bind() {
	glBindTexture(GL_TEXTURE_2D, m_handle);
}

void sgl::texture::unbind() {
	glBindTexture(GL_TEXTURE_2D, 0); // TODO: bind previously bound texture?
}

void sgl::texture::set_filter(filter minmag) {
	set_filter(minmag, minmag);
}

void sgl::texture::set_filter(filter min, filter mag) {
	bind();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, static_cast<GLenum>(min));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, static_cast<GLenum>(mag));
	unbind();
}
#include <stdio.h>
bool sgl::texture::load(int width, int height, sgl::texture::internalformat internal_format, void* data,
                        sgl::texture::format loader_format, sgl::texture::datatype dtype) {
	m_format_internal = static_cast<GLint>(internal_format);
	m_format_loader = static_cast<GLenum>(loader_format);
	m_datatype = static_cast<GLenum>(dtype);
	m_width = width;
	m_height = height;

	bind();

	glTexImage2D(GL_TEXTURE_2D, 0, m_format_internal, width, height, 0, m_format_loader, m_datatype, data);

	set_filter(sgl::texture::filter::linear, sgl::texture::filter::nearest);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	unbind();

	return true;
}

void sgl::texture::resize(int width, int height) {
	if (!width)
		width = m_width;
	if (!height)
		height = m_height;

	// only resize if size changed.
	if (width == m_width && height == m_height)
		return;
	m_width = width;
	m_height = height;

	bind();
	glTexImage2D(GL_TEXTURE_2D, 0, m_format_internal, width, height, 0, m_format_loader, m_datatype, nullptr);
	unbind();
}

GLuint sgl::texture::get_texture() const {
	return m_handle;
}

sgl::texture::operator GLuint() const {
	return m_handle;
}
