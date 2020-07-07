#pragma once

#include <GL/gl3w.h>

namespace sgl {
	class renderbuffer {
	public:
		/**
		 * @brief Specifies the renderbuffers pixel store format.
		 */
		enum class internalformat : GLenum {
			r             = GL_RED,                   /**< Store 1 component in the red channel. */
			rg            = GL_RG,                    /**< Store 2 components in the red and green channels. */
			rgb           = GL_RGB,                   /**< Store 3 components in the red, green and blue channels. */
			rgba          = GL_RGBA,                  /**< Store 4 components, RGB with alpha. Default. */
			depth         = GL_DEPTH_COMPONENT,       /**< Store 1 component, the depth value. */
			depth16       = GL_DEPTH_COMPONENT16,     /**< Store 1 component, the depth value. */
			depth24       = GL_DEPTH_COMPONENT24,     /**< Store 1 component, the depth value. */
			depth32f      = GL_DEPTH_COMPONENT32F,    /**< Store 1 component, the depth value. */
			depth_stencil = GL_DEPTH_STENCIL,         /**< Store 2 components, depth and stencil. */
			depth24_stencil8 = GL_DEPTH24_STENCIL8,	  /**< Store 2 components, depth and stencil. */
			depth32f_stencil8 = GL_DEPTH32F_STENCIL8, /**< Store 2 components, high precision depth and stencil. */
		};
		
	public:
		
		renderbuffer();
		~renderbuffer();
		
		void create(int width, int height, internalformat format);
		
		void bind();
		void unbind();
		
		operator GLuint() const;
	private:
		GLuint m_handle;
		int m_width, m_height;
		
	};
}
