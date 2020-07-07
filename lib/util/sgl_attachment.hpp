#pragma once

#include <GL/gl3w.h>

namespace sgl {
	
	/**
	 * @brief Specifies the framebuffer attachment type.
	 * 
	 */
	class attachment {
	public:
		
		/**
		 * @brief The attachment type.
		 */
		enum class type : GLenum {
			color         = GL_COLOR_ATTACHMENT0,        /**< A color attachment. */
			depth         = GL_DEPTH_ATTACHMENT,         /**< The depth buffer attachment. */
			stencil       = GL_STENCIL_ATTACHMENT,       /**< The stencil buffer attachment. */
			depth_stencil = GL_DEPTH_STENCIL_ATTACHMENT  /**< The stencil and depth buffer attachment. */
		};
		
	public:
		static attachment color(int i = 0);
		static attachment depth_stencil();
		
		attachment(type atype);
		
		operator GLenum() const;
	private:
		GLenum m_type;
	};
	
}
