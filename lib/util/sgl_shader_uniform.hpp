#ifndef SGL_SHADER_UNIFORM_HPP
#define SGL_SHADER_UNIFORM_HPP

#include <GL/gl3w.h>

#include <glm/glm.hpp>

namespace sgl {
	
	/**
	 * @brief References a specific uniform location in a shader.
	 *
	 * Wraps access to a shader uniform allowing a much cleaner
	 * and straightforward way of writing to uniforms.
	 * Typically used in combination with sgl::shader#operator[]().
	 * 
	 * @code{.cpp}
	 *     #include <sgl_shader.hpp>
	 *     #include <sgl_shader_uniform.hpp>
	 *     
	 *     sgl::shader shader("vert.glsl", "frag.glsl");
	 * 
	 *     // Accessing a uniform
	 *     shader["Normal"] = glm::vec3(1.0f, 0.0f, 0.0f);
	 *     
	 *     // Accessing a uniform referenced by its location
	 *     GLint uMVP = shader["MVP"];
	 *     shader[uMVP] = glm::mat4(1.0f);
	 *
	 *     // Accessing a sgl::shader_uniform
	 *     sgl::shader_uniform uPassedTime = shader["PassedTime"];
	 *     uPassedTime = (float)get_current_time();
	 *
	 * @endcode
	 * 
	 * Setting the value of the current uniform with
	 * sgl::shader_uniform#operator=() will switch the
	 * currenty active shader program, but reactivate
	 * the previous program immediately after updating
	 * the uniform.
	 * 
	 * @see sgl::shader[]
	 */
	class shader_uniform {
		const GLuint shader_program;
		const GLint uniform_location;

		/* save active program before switching to this->shader_program and restore later */
		GLint saved_program;
		void use_program();
		void restore_previous_program();

	public:
		/**
		 * @brief Create a reference to a specific uniform location in a program.
		 * @param shader_program The shader program.
		 * @param location The uniforms location in the shader program.
		 * @see sgl::shader[]
		 */
		shader_uniform(const GLuint shader_program, const GLint location);
		
		/**
		 * @brief Returns the uniform location inside the shader program.
		 * @return The uniform location.
		 */
		operator int() const;
		
		/* scalar */
		/**
		 * @brief Sets a uniform variable of type bool.
		 */
		void operator=(const GLboolean &b);
		/**
		 * @brief Sets a uniform variable of type int.
		 */
		void operator=(const GLint &i);
		/**
		 * @brief Sets a uniform variable of type unsigned int.
		 */
		void operator=(const GLuint &u);
		/**
		 * @brief Sets a uniform variable of type float.
		 */
		void operator=(const GLfloat &v);

		/* vector */
		/**
		 * @brief Sets a uniform variable of type vec2
		 */
		void operator=(const glm::vec2 &vec2);
		/**
		 * @brief Sets a uniform variable of type vec3
		 */
		void operator=(const glm::vec3 &vec3);
		/**
		 * @brief Sets a uniform variable of type vec4
		 */
		void operator=(const glm::vec4 &vec4);

		/* matrix */
		/**
		 * @brief Sets a uniform variable of type mat2
		 */
		void operator=(const glm::mat2 &mat2);
		/**
		 * @brief Sets a uniform variable of type mat3
		 */
		void operator=(const glm::mat3 &mat3);
		/**
		 * @brief Sets a uniform variable of type mat4
		 */
		void operator=(const glm::mat4 &mat4);
		
	};

}

#endif
