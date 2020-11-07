#include "sgl/sgl_framebuffer.hpp"

////////////
// PUBLIC //
////////////

sgl::framebuffer::framebuffer() {
	glGenFramebuffers(1, &m_handle);
}

sgl::framebuffer::~framebuffer() {
	glDeleteFramebuffers(1, &m_handle);
}

void sgl::framebuffer::bind(sgl::framebuffer::target target) {
	glBindFramebuffer(static_cast<GLenum>(target), m_handle);
}

void sgl::framebuffer::unbind(framebuffer::target target) {
	glBindFramebuffer(static_cast<GLenum>(target), 0); // TODO: bind previous instead of default fb.
}

void sgl::framebuffer::attach(sgl::texture &texture, sgl::attachment attachment, framebuffer::target target) {
	m_attachments.push_back(attachment);
	
	bind(target);
	glFramebufferTexture2D(static_cast<GLenum>(target), attachment, GL_TEXTURE_2D, texture, 0);
	unbind(target);
}

void sgl::framebuffer::attach(sgl::renderbuffer &renderbuffer, sgl::attachment attachment, framebuffer::target target) {
	m_attachments.push_back(attachment);
	
	bind(target);
	glFramebufferRenderbuffer(static_cast<GLenum>(target), attachment, GL_RENDERBUFFER, renderbuffer);
	unbind(target);
}

void sgl::framebuffer::targets(std::vector<sgl::attachment> atts) {
	auto &attachments = atts;
	if (atts.size() == 0) {
		attachments = m_attachments;
	}
	
	std::vector<GLenum> colorbuffers;
	for (auto a : attachments) {
		if (a.is_color()) {
			colorbuffers.push_back(a);
		}
	}
	
	// actually set the draw buffers
	bind(sgl::framebuffer::target::draw);
	glDrawBuffers(colorbuffers.size(), &colorbuffers[0]);
	unbind(sgl::framebuffer::target::draw);
}

bool sgl::framebuffer::is_complete(framebuffer::target target) {
	bind(target);
	GLenum status = glCheckFramebufferStatus(static_cast<GLenum>(target));
	unbind(target);
	
	return status == GL_FRAMEBUFFER_COMPLETE;
}

/////////////
// PRIVATE //
/////////////
