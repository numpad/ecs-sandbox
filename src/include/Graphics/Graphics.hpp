#pragma once

#include <GL/gl3w.h>
#include "Graphics/GLState.hpp"

class Graphics {
public:

	void initialize();

	void resetState();
	void setState(const GLState& state);

private:
	GLState m_currentGlState;

	void setDepthTest(const bool new_depth_test);
	void setDepthWrite(const bool new_depth_write);
	void setCullFace(const bool new_cull_face);
	void setBlend(const bool new_blend);
	void setBlendFunc(const int src, const int dst, const int src_alpha, const int dst_alpha);
};
