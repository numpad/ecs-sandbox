#ifndef SGL_SHADER_HPP
#define SGL_SHADER_HPP

#include <string.h>

#include <iostream>
#include <string>
#include <fstream>

#include <GL/gl3w.h>

#include "sgl_shader_uniform.hpp"

namespace sgl {
	
	/**
	 * @brief Combines and simplifies loading and using a shader program.
	 *
	 * The 3 goals of this class are eleminating the tedious process
	 * of @em loading, compiling & linking a shader program (and handling possible errors),
	 * enabling access to @em uniform variables (in combination with sgl::shader_uniform)
	 * in a more intuitive and straightforward fashion and seamless @em integration
	 * with plain OpenGL function calls. (Such as to glUniform*)
	 * 
	 * @code{.cpp}
	 *     #include <sgl_shader.hpp>
	 *     
	 *     // Loading a shader program:
	 *     sgl::shader shader;
	 *     shader.load("vert.glsl", sgl::shader::VERTEX);
	 *     shader.load("frag.glsl", sgl::shader::FRAGMENT);
	 *     shader.compile(sgl::shader::VERTEX);
	 *     shader.compile(sgl::shader::FRAGMENT);
	 *     shader.link();
	 *
	 *     // Or using the much simpler but less verbose, modular and flexible shorthand:
	 *     sgl::shader shader("vert.glsl", "frag.glsl");
	 *     
	 *     
	 *     // Setting uniform variables with the flexible sgl::shader_uniform
	 *     // Note: Updating the value of a uniform does not require
	 *     //       manually switching the currently active program.
	 *	   //       sgl::shader_uniform::operator=() switches to the right
	 *     //       program and later restores the previously active one.
	 *     shader["Position"]      = glm::vec3(...);
	 *     shader["PassedTime"]    = (float)get_current_time();
	 *     shader["NormalTexture"] = 0;
	 *     
	 *     // Finding the unicorn location beforehand for faster access
	 *     sgl::shader_uniform uPosition = shader["Position"];
	 *     while (game_is_running()) {
	 *         uPosition = glm::vec3(...);
	 *     }
	 *
	 *     // Integration with OpenGL
	 *     GLint uPassedTime = shader["PassedTime"];
	 *     glUseProgram(shader);
	 *     glUniform1f(uPassedTime, (float)get_current_time());
	 *     
	 * @endcode
	 *
	 * @see sgl::shader_uniform
	 */
	class shader {
	public:
		/**
		 * @brief Indicating on which shader to operate.
		 * 
		 */
		enum type {
			VERTEX,		/**< Operate on the vertex shader. */
			FRAGMENT,	/**< Operate on the fragment shader. */
			GEOMETRY,	/**< Operate on the geometry shader. */
			MAX_TYPES,	/**< Use this to iterate over every shader type.  */
			/* special values */
			WILDCARD	/**< Wildcard, operate on all shaders that are usable. */
		};
		
	private:
		
		enum shader_stage {
			STAGE_EMPTY,
			STAGE_LOADED,
			STAGE_COMPILED,
			STAGE_LINKED
		};

		/* translate sgl::shader::type to OpenGL shader type */
		GLenum type_to_gl_shader[sgl::shader::MAX_TYPES] = {
			GL_VERTEX_SHADER,
			GL_FRAGMENT_SHADER,
			GL_GEOMETRY_SHADER
		};
		
		std::string type_to_name[sgl::shader::MAX_TYPES] = {
			"vertex",
			"fragment",
			"geometry"
		};
		
		shader_stage stage_of_type[sgl::shader::MAX_TYPES];
		char *shaders_src[sgl::shader::MAX_TYPES];
		GLuint shaders[sgl::shader::MAX_TYPES];
		GLuint program;
		
		std::string filenames[sgl::shader::MAX_TYPES];
		
		static std::string load_file(std::string fname);
		
	public:
		
		/**
		 * @brief Create an empty shader.
		 *
		 * Creates an empty OpenGL shader program, which still needs
		 * to be loaded, compiled and linked. This allows for much more
		 * modular but verbose code.
		 *
		 * @code{.cpp}
		 *     #include <sgl_shader.hpp>
		 *     
		 *     // Load a vertex and fragment shader...
		 *     sgl::shader shader;
		 *     bool load_vert_ok = shader.load("vert.glsl", sgl::shader::VERTEX);
		 *     bool load_frag_ok = shader.load("frag.glsl", sgl::shader::FRAGMENT);
		 *     // ...compile both shaders...
		 *     bool cmpl_vert_ok = shader.compile(sgl::shader::VERTEX);
		 *     bool cmpl_frag_ok = shader.compile(sgl::shader::FRAGMENT);
		 *     // ...and finally link the shader program.
		 *     bool link_ok = shader.link();
		 * 
		 * @endcode
		 * 
		 */
		shader();
		
		/**
		 * @brief Loads, compiles and links a shader program.
		 * 
		 * @warning The arguments are positional; passing anything other
		 *          than a vertex shader filename followed by a fragment
		 *          shader filename will lead to undefined behavior.
		 *
		 * Loads a shader program from a given vertex and fragment shader. To use
		 * a more modular approach construct the shader with sgl::shader#shader()
		 * and sgl::shader#load(), sgl::shader#compile() and sgl::shader#link().
		 * 
		 * @code{.cpp}
		 *     #include <sgl_shader.hpp>
		 *     
		 *     // Load a vertex and fragment shader, less control and modularity
		 *     // but much cleaner code.
		 *     sgl::shader shader("vert.glsl", "frag.glsl");
		 *     
		 * @endcode
		 * 
		 */
		shader(std::string fname_vert, std::string fname_frag);
		~shader();
		
		/**
		 * @brief Unloads all resources reserved by the shader and deletes the program.
		 * 
		 * Calling sgl::shader#unload() will free all reserved memory and afterwards
		 * delete the shader program. This is automatically done on destruction, however
		 * for example hotswapping can make much use of this.
		 *
		 * @code{.cpp}
		 *     #include <sgl_shader.hpp>
		 *     
		 *     // Load a shader program
		 *     sgl::shader shader("vert.glsl", "frag.glsl");
		 *
		 *     // Render a button to reload the shader when pressed
		 *     if (ImGui::Button("Hotswap shader")) {
		 *         // Unload and delete the shader program
		 *         shader.unload();
		 *
		 *         // Reload the previous shader.
		 *         shader.load("vert.glsl", "frag.glsl");
		 *     }
		 *
		 * @endcode
		 *
		 * @see sgl::shader#~shader()
		 */
		void unload();
		
		/**
		 * TODO: documentation!!!!
		 */
		void reload();
		
		/**
		 * @brief Allow simple and clean interoperability between sgl::shader and OpenGL.
		 *
		 * An important goal of simgl is to blend the lines between the wrappers it provides
		 * and OpenGL. sgl::shader is a great example for this philosophy and approach; it
		 * allows loading a shader program and accessing its uniform variables using a
		 * simple and clean, object oriented approach yet doesn't stop the user from
		 * working with the shader program using bare OpenGL.
		 *
		 * @code{.cpp}
		 *     #include <sgl_shader.hpp>
		 *     
		 *     // Load a shader program
		 *     sgl::shader shader("vert.glsl", "frag.glsl");
		 *     
		 *     // Updating a uniform with sgl::shader_uniform
		 *     shader["PassedTime"] = (float)get_current_time();
		 *     
		 *     // Updating a uniform with bare OpenGL
		 *     GLint location = glGetUniformLocation(shader, "PassedTime");
		 *     glUseProgram(shader);
		 *     glUniform1f(location, (float)get_current_time());
		 * @endcode
		 * 
		 * Since @em OpenGL @em interoperability is a high priority of this project,
		 * simgl encourages mixing abstracted sgl::shader operations with
		 * bare OpenGL to produce clean, readable code while retaining in
		 * full control over performance and the lower-level interface.
		 * 
		 * @return The OpenGL shader program.
		 */
		operator GLuint() const;
		
		/**
		 * @brief Use the shader.
		 * 
		 * @todo
		 */
		void use();

		/**
		 * @brief Get the location of the uniform @em uniform_name.
		 *
		 * It is encouraged working with sgl::shader#operator[]() and using sgl::shader_uniform
		 * to produce cleaner code that can be used with both simgl and bare OpenGL.
		 * Reasos in favor of using this are explicitly encouraging bare OpenGL code
		 * and more verbose code.
		 * 
		 * @code{.cpp}
		 *     #include <sgl_shader.hpp>
		 *     
		 *     sgl::shader shader(...);
		 *     
		 *     // Accessing uniforms using the verbose sgl::shader#uniform()
		 *     GLint uMVP = shader.uniform("MVP");
		 *     
		 *     // Accessing uniforms using the cleaner sgl::shader#operator[]()
		 *     GLint uMVP = shader["MVP"];
		 * @endcode
		 * 
		 * @param uniform_name The name of the uniform.
		 * @return The location of the uniform inside the shader. See glGetUniformLocation()
		 */
		GLint uniform(std::string uniform_name) const;
		/** @copydoc sgl::shader::uniform(std::string uniform_name) const */
		GLint uniform(const char *uniform_name) const;
		
		/**
		 * @brief Access a uniform inside a shader referenced by its shader location.
		 *
		 * The location is usually retreived once from sgl::shader#operator[](std::string), sgl::shader#uniform()
		 * or glGetUniformLocation() and used to quickly update uniforms were performance is more
		 * critical.
		 * 
		 * @code{.cpp}
		 *     #include <sgl_shader.hpp>
		 *     
		 *     sgl::shader shader("vert.glsl", "frag.glsl");
		 *     
		 *     // Get the location of the uniform "PassedTime"
		 *     GLint uPassedTime = shader["PassedTime"];
		 *
		 *     // Run our game loop, performance critical
		 *     window.update([]() {
		 *         shader[uPassedTime] = (float)get_current_time();
		 *     });
		 * @endcode
		 * 
		 * @param location Uniform location inside shader.
		 * @return A sgl::shader_uniform referencing the uniform at @em location inside the current shader.
		 * @see sgl::shader_uniform
		 */
		sgl::shader_uniform operator[](GLint location);
		
		/**
		 * @brief Access a uniform inside a shader referenced by its name.
		 * 
		 * Uniform access that isn't time/performance critical is encouraged
		 * to use the cleaner sgl::shader_uniform#operator[](const char *uniform_name).
		 * 
		 * @code{.cpp}
		 *     #include <sgl_shader.hpp>
		 *     
		 *     sgl::shader shader("vert.glsl", "frag.glsl");
		 *     
		 *     // Initializes a uniform value
		 *     shader["Viewport"] = glm::vec2(window.width, window.height);
		 *     
		 * @endcode
		 * 
		 * @param uniform_name Uniform name inside shader.
		 * @return A sgl::shader_uniform referencing the uniform named @em uniform_name inside the current shader.
		 * @see sgl::shader_uniform
		 */
		sgl::shader_uniform operator[](const char *uniform_name);
		
		/**
		 * @brief Loads a shader of type @em type from file at @em fname.
		 * 
		 * Verbose and modular approach of manually loading a shader, needs to be compiled
		 * and linked afterwards. Unless modularity is required, sgl::shader#shader(std::string, std::string)
		 * is preferred to improve readability.
		 * 
		 * @param fname Path, filename of shader source.
		 * @param type Shader type (e.g. vertex shader, fragment shader)
		 * @return true if the shader successfully loaded.
		 * @see sgl::shader#load_from_memory()
		 * @see sgl::shader#compile()
		 * @see sgl::shader#link()
		 */
		bool load(std::string fname, sgl::shader::type type);
		
		/**
		 * @brief Loads a shader of type @em type from memory.
		 * 
		 * Load a shader already in memory in form of a std::string.
		 * Mostly used to include a hardcoded shader or using a custom
		 * file loader.
		 * Needs to be compiled and linked after loading.
		 * 
		 * @param ssrc Shader source code.
		 * @param type Shader type (e.g. vertex shader, fragment shader)
		 * @return true if the shader successfully loaded.
		 * @see sgl::shader#load()
		 * @see sgl::shader#compile()
		 * @see sgl::shader#link()
		 */
		bool load_from_memory(std::string ssrc, sgl::shader::type type);

		/**
		 * @brief Compiles one (or all, by default) previously loaded shaders of sgl::shader#type @em type.
		 * 
		 * Compile previously loaded shader, needs to be linked afterwards.
		 * Errors during compilation will be printed to std::cout. 
		 * 
		 * @param type Type of the shader to compile.
		 * @return true if the shader successfully compiled.
		 * 
		 * @pre Can only compile already @link sgl::shader#load() loaded @endlink shaders.
		 *
		 * @see sgl::shader#load()
		 * @see sgl::shader#load_from_memory()
		 * @see sgl::shader#link()
		 */
		bool compile(sgl::shader::type type = sgl::shader::WILDCARD);
		
		/**
		 * @brief Links previously loaded shaders to a shader program.
		 *
		 * Link several previously loaded and compiled shaders.
		 * Errors during linking will be printed to std::cout.
		 *
		 * @return true if the shader program successfully linked.
		 *
		 * @pre Can only link if every shader has been @link sgl::shader#load() loaded @endlink
		 *      and @link sgl::shader#compile() compiled @endlink.
		 *
		 * @see sgl::shader#load()
		 * @see sgl::shader#load_from_memory()
		 * @see sgl::shader#compile()
		 */
		bool link();
	};

}

#endif

