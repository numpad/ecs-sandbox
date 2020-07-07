#include "sgl_attachment.hpp"

sgl::attachment sgl::attachment::color(int i) {
	auto a = sgl::attachment(sgl::attachment::type::color);
	a.m_type += i;
	return a;
}

sgl::attachment sgl::attachment::depth_stencil() {
	return sgl::attachment(sgl::attachment::type::depth_stencil);
}

sgl::attachment::attachment(type atype)
 : m_type{static_cast<GLenum>(atype)}
{
	
}
		
sgl::attachment::operator GLenum() const {
	return m_type;
}
