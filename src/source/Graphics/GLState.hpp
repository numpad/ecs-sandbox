#pragma once

#include <optional>
#include <GL/gl3w.h>

struct GLState {
	bool depth_test = false;
	bool depth_write = false;
	bool cull_face = false;
	bool blend = false;
	int blend_src = GL_SRC_ALPHA;
	int blend_dst = GL_ONE_MINUS_SRC_ALPHA;
	int blend_src_alpha = GL_ONE;
	int blend_dst_alpha = GL_ONE_MINUS_SRC_ALPHA;

	/**
	 * Queries OpenGL using `glGet*` and returns this state.
	 *
	 * @returns The currently active OpenGL state.
	 */
	static GLState Current() {
		GLState state;
		// depth test
		GLboolean _depth_test;
		glGetBooleanv(GL_DEPTH_TEST, &_depth_test);
		state.depth_test = (_depth_test == GL_TRUE);
		// depth mask
		GLboolean _depth_mask;
		glGetBooleanv(GL_DEPTH_WRITEMASK, &_depth_mask);
		state.depth_write = (_depth_mask == GL_TRUE);
		// cull face
		GLboolean _cull_face;
		glGetBooleanv(GL_CULL_FACE, &_cull_face);
		state.cull_face = (_cull_face == GL_TRUE);
		// blend
		GLboolean _blend;
		glGetBooleanv(GL_BLEND, &_blend);
		state.blend = (_blend == GL_TRUE);
		// blend func
		GLint _blend_src, _blend_dst, _blend_src_alpha, _blend_dst_alpha;
		glGetIntegerv(GL_BLEND_SRC_RGB, &_blend_src);
		glGetIntegerv(GL_BLEND_DST_RGB, &_blend_dst);
		glGetIntegerv(GL_BLEND_SRC_ALPHA, &_blend_src_alpha);
		glGetIntegerv(GL_BLEND_DST_ALPHA, &_blend_dst_alpha);
		state.blend_src = _blend_src;
		state.blend_dst = _blend_dst;
		state.blend_src_alpha = _blend_src_alpha;
		state.blend_dst_alpha = _blend_dst_alpha;
		return state;
	}
};
