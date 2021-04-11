#pragma once

#include <GL/gl3w.h>

namespace sgl {
	
	class texture {
	public:
		/**
		 * @brief Specifies the textures pixel store format.
		 */
		enum class format : GLenum {
			r             = GL_RED,             /**< Store 1 component in the red channel. */
			rg            = GL_RG,              /**< Store 2 components in the red and green channels. */
			rgb           = GL_RGB,             /**< Store 3 components in the red, green and blue channels. */
			rgba          = GL_RGBA,            /**< Store 4 components, RGB with alpha. Default. */
			depth         = GL_DEPTH_COMPONENT, /**< Store 1 component, the depth component. */
			stencil       = GL_STENCIL_INDEX,   /**< Store 1 component, the stencil index. */
			depth_stencil = GL_DEPTH_STENCIL    /**< Store 2 components, depth and stencil. */
		};
		
		/**
		 * @brief Specifies the textures pixel store format.
		 */
		enum class internalformat : GLint {
			r             = GL_RED,                   /**< Store 1 component in the red channel. */
			rg            = GL_RG,                    /**< Store 2 components in the red and green channels. */
			rgb           = GL_RGB,                   /**< Store 3 components in the red, green and blue channels. */
			rgba          = GL_RGBA,                  /**< Store 4 components, RGB with alpha. Default. */
			srgb          = GL_SRGB,                  /**< Store 3 components, automatically converting them to linear colorspace. */
			srgba         = GL_SRGB_ALPHA,            /**< Store 4 components, automatically converting them to linear colorspace. */
			r8            = GL_R8,                    /**< Store 1 8bit component in the red channel. */
			rg8           = GL_RG8,                   /**< Store 2 8bit components in the red and green channels. */
			rgb8          = GL_RGB8,                  /**< Store 3 8bit components in the red, green and blue channels. */
			rgba8         = GL_RGBA8,                 /**< Store 4 8bit components, RGB with alpha. Default. */
			r16f          = GL_R16F,                  /**< Store 1 16bit float component in the red channel. */
			rg16f         = GL_RG16F,                 /**< Store 2 16bit float components in the red and green channels. */
			rgb16f        = GL_RGB16F,                /**< Store 3 16bit float components in the red, green and blue channels. */
			rgba16f       = GL_RGBA16F,               /**< Store 4 16bit float components, RGB with alpha. Default. */
			r32f          = GL_R32F,                  /**< Store 1 32bit float component in the red channel. */
			rg32f         = GL_RG32F,                 /**< Store 2 32bit float components in the red and green channels. */
			rgb32f        = GL_RGB32F,                /**< Store 3 32bit float components in the red, green and blue channels. */
			rgba32f       = GL_RGBA32F,               /**< Store 4 32bit float components, RGB with alpha. Default. */
			depth         = GL_DEPTH_COMPONENT,       /**< Store 1 component, the depth value. */
			depth16       = GL_DEPTH_COMPONENT16,     /**< Store 1 component, the depth value. */
			depth24       = GL_DEPTH_COMPONENT24,     /**< Store 1 component, the depth value. */
			depth32f      = GL_DEPTH_COMPONENT32F,    /**< Store 1 component, the depth value. */
			depth_stencil = GL_DEPTH_STENCIL,         /**< Store 2 components, depth and stencil. */
			depth24_stencil8 = GL_DEPTH24_STENCIL8,	  /**< Store 2 components, depth and stencil. */
			depth32f_stencil8 = GL_DEPTH32F_STENCIL8  /**< Store 2 components, high precision depth and stencil. */
		};
		
		enum class datatype : GLenum {
			u8    = GL_UNSIGNED_BYTE,
			s8    = GL_BYTE,
			u16   = GL_UNSIGNED_SHORT,
			s16   = GL_SHORT,
			u24_8 = GL_UNSIGNED_INT_24_8,
			u32   = GL_UNSIGNED_INT,
			s32   = GL_INT,
			f32   = GL_FLOAT
		};
		
		/**
		 * @brief Specifies which filter to use when reading pixel data.
		 */
		enum class filter : GLenum {
			nearest         = GL_NEAREST,                /**< Uses the nearest pixel. */
			linear          = GL_LINEAR,                 /**< Uses the weighted average of the nearest 4 pixels. */
			nearest_nearest = GL_NEAREST_MIPMAP_NEAREST, /**< Uses the nearest pixel and the nearest mipmap. */
			linear_linear   = GL_LINEAR_MIPMAP_LINEAR,   /**< Uses the weighted average of the nearest 4 pixels and the weighted average of the nearest 2 mipmaps. */
			nearest_linear  = GL_NEAREST_MIPMAP_LINEAR,  /**< Uses the nearest pixel and the weighted average of the nearest 2 mipmaps. */
			linear_nearest  = GL_LINEAR_MIPMAP_NEAREST   /**< Uses the weighted average of the nearest 4 pixels and the nearest mipmap. */
		};
	public:
		
		texture();
		~texture();
		
		/**
		 * @brief Bind the texture object.
		 */
		void bind();
		/**
		 * @brief Unbind the texture object.
		 */
		void unbind();
		
		void set_filter(filter minmag);
		void set_filter(filter min, filter mag);
		
		bool load(int width, int height, internalformat internal_format = internalformat::rgba, void *data = nullptr, format loader_format = format::rgba, datatype dtype = datatype::f32);
		
		/**
		 * @brief Resizes the texture if width and/or height are different, will not stretch/shrink existing image data.
		 * 
		 * @warning Can only be used after a successful call to sgl::texture#load().
		 * 
		 * @param width The new width. A value of 0 indicates no change.
		 * @param height The new height. A value of 0 indicates no change.
		 */
		void resize(int width, int height);
		
		/**
		 * @brief Get the OpenGL texture name.
		 * 
		 * @code{.cpp}
		 *     #include <sgl_texture.hpp>
		 *     
		 *     sgl::texture texture;
		 *     texture.load(512, 512, nullptr);
		 *     
		 *     // Use the sgl::texture object as raw GLuint.
		 *     glBindTexture(GL_TEXTURE_2D, texture.get_texture());
		 *     
		 * @endcode
		 * @return The OpenGL texture name as GLuint.
		 */
		 GLuint get_texture() const;
		 
		 /**
		 * @brief Get the OpenGL texture name.
		 * 
		 * Implicit cast the sgl::texture to its respective OpenGL texture object.
		 * 
		 * @code{.cpp}
		 *     #include <sgl_texture.hpp>
		 *     
		 *     sgl::texture texture;
		 *     texture.load(512, 512, nullptr);
		 *     
		 *     // Use the sgl::texture object as raw GLuint.
		 *     glBindTexture(GL_TEXTURE_2D, texture);
		 *     
		 * @endcode
		 * @return The OpenGL texture name as GLuint.
		 */
		operator GLuint() const;
		
	private:
		GLint m_format_internal;
		GLenum m_format_loader;
		GLenum m_datatype;
		
		GLuint m_handle;
		int m_width, m_height;
		
	};
}
