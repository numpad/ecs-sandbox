#include "sgl_attachment.hpp"

sgl::attachment sgl::attachment::color(int i) {
	#if CFG_DEBUG
	GLint max_colors;
	glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &max_colors);
	if (i >= max_colors) {
		std::cout << "[ERR] GPU only supports " << max_colors << " framebuffer color attachments." << std::endl;
		throw "too_many_attachments";
	}
	#endif
	
	auto a = sgl::attachment(sgl::attachment::type::color);
	a.m_type += i;
	return a;
}

sgl::attachment sgl::attachment::depth_stencil() {
	return sgl::attachment(sgl::attachment::type::depth_stencil);
}

sgl::attachment::attachment(type atype)
 : m_type{static_cast<GLenum>(atype)},
   m_basetype{atype}
{
	
}
		
sgl::attachment::operator GLenum() const {
	return m_type;
}
