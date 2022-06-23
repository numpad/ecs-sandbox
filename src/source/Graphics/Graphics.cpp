#include "Graphics/Graphics.hpp"

////////////
// PUBLIC //
////////////

void Graphics::initialize() {
	m_currentGlState = GLState::Current();

}

void Graphics::setState(const GLState& state) {
	setDepthTest(state.depth_test);
	setDepthWrite(state.depth_write);
	setCullFace(state.cull_face);
	setBlend(state.blend);
	setBlendFunc(state.blend_src, state.blend_dst, state.blend_src_alpha, state.blend_dst_alpha);
}

/////////////
// PRIVATE //
/////////////

void Graphics::setDepthTest(const bool new_depth_test) {
	if (m_currentGlState.depth_test == new_depth_test) return;

	m_currentGlState.depth_test = new_depth_test;
	m_currentGlState.depth_test ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
}

void Graphics::setDepthWrite(const bool new_depth_write) {
	if (m_currentGlState.depth_write == new_depth_write) return;

	m_currentGlState.depth_write = new_depth_write;
	glDepthMask(m_currentGlState.depth_write);
}

void Graphics::setCullFace(const bool new_cull_face) {
	if (m_currentGlState.cull_face == new_cull_face) return;

	m_currentGlState.cull_face = new_cull_face;
	m_currentGlState.cull_face ? glEnable(GL_CULL_FACE) : glDisable(GL_CULL_FACE);
}

void Graphics::setBlend(const bool new_blend) {
	if (m_currentGlState.blend == new_blend) return;

	m_currentGlState.blend = new_blend;
	m_currentGlState.blend ? glEnable(GL_BLEND) : glDisable(GL_BLEND);
}

void Graphics::setBlendFunc(const int src, const int dst, const int src_alpha, const int dst_alpha) {
	if (m_currentGlState.blend_src == src && m_currentGlState.blend_dst == dst && m_currentGlState.blend_src_alpha == src_alpha && m_currentGlState.blend_dst_alpha == dst_alpha) return;

	m_currentGlState.blend_src = src;
	m_currentGlState.blend_dst = dst;
	m_currentGlState.blend_src_alpha = src_alpha;
	m_currentGlState.blend_dst_alpha = dst_alpha;
	glBlendFuncSeparate(m_currentGlState.blend_src, m_currentGlState.blend_dst, m_currentGlState.blend_src_alpha, m_currentGlState.blend_dst_alpha);
}
