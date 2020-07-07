#pragma once

#include <GL/gl3w.h>

#include "sgl_texture.hpp"
#include "sgl_renderbuffer.hpp"
#include "sgl_attachment.hpp"

namespace sgl {
	
	class framebuffer {
		public:
			/**
			 * @brief Indicates the framebuffer target for some operations.
			 */
			enum class target : GLenum {
				both = GL_FRAMEBUFFER,      /**< Operate on both read and draw framebuffers. */
				read = GL_READ_FRAMEBUFFER, /**< Operate on the read framebuffer. */
				draw = GL_DRAW_FRAMEBUFFER  /**< Operate on the draw framebuffer. */
			};
			
		public:
			
			framebuffer();
			~framebuffer();
			
			/**
			 * @brief Binds the framebuffer.
			 * @param target Specifies the framebuffer target to bind.
			 * 
			 * @see sgl::framebuffer#target
			 */
			void bind(framebuffer::target target = framebuffer::target::both);
			
			/**
			 * @brief Unbinds the framebuffer.
			 * 
			 * This is the same as binding the previously bound framebuffer target.
			 * If no framebuffer was bound to this target before the default framebuffer will be used.
			 * 
			 * @param target Specifies which framebuffer target to unbind.
			 */
			void unbind(framebuffer::target target = framebuffer::target::both);
			
			void attach(sgl::texture      &texture,      sgl::attachment attachment, framebuffer::target target = framebuffer::target::both);
			void attach(sgl::renderbuffer &renderbuffer, sgl::attachment attachment, framebuffer::target target = framebuffer::target::both);
			
			/**
			 * @brief Returns if the framebuffer is complete and fully usable.
			 * @details [long description]
			 * @return true if the framebuffer is 
			 */
			bool is_complete(framebuffer::target target = framebuffer::target::both);
		private:
			GLuint m_handle;
			
	};
	
}
