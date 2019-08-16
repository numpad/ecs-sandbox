#include "config.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <FastNoise/FastNoise.h>
#include <stb/stb_image.h>
#include <entt/entt.hpp>
#include <util/sgl_shader.hpp>

#include <World.hpp>

void onResize(GLFWwindow *, int width, int height) {
	glViewport(0, 0, width, height);
}

void APIENTRY glDebugOutput(GLenum source,
                            GLenum type,
                            GLuint id,
                            GLenum severity,
                            GLsizei length,
                            const GLchar *message,
                            const void *userParam)
{
	
    // ignore non-significant error/warning codes
    if(id == 131169 || id == 131185 || id == 131218 || id == 131204) return; 

    std::cout << "---------------" << std::endl;
    std::cout << "Debug message (" << id << "): " <<  message << std::endl;

    switch (source)
    {
        case GL_DEBUG_SOURCE_API:             std::cout << "Source: API"; break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   std::cout << "Source: Window System"; break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER: std::cout << "Source: Shader Compiler"; break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:     std::cout << "Source: Third Party"; break;
        case GL_DEBUG_SOURCE_APPLICATION:     std::cout << "Source: Application"; break;
        case GL_DEBUG_SOURCE_OTHER:           std::cout << "Source: Other"; break;
    } std::cout << std::endl;

    switch (type)
    {
        case GL_DEBUG_TYPE_ERROR:               std::cout << "Type: Error"; break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: std::cout << "Type: Deprecated Behaviour"; break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  std::cout << "Type: Undefined Behaviour"; break; 
        case GL_DEBUG_TYPE_PORTABILITY:         std::cout << "Type: Portability"; break;
        case GL_DEBUG_TYPE_PERFORMANCE:         std::cout << "Type: Performance"; break;
        case GL_DEBUG_TYPE_MARKER:              std::cout << "Type: Marker"; break;
        case GL_DEBUG_TYPE_PUSH_GROUP:          std::cout << "Type: Push Group"; break;
        case GL_DEBUG_TYPE_POP_GROUP:           std::cout << "Type: Pop Group"; break;
        case GL_DEBUG_TYPE_OTHER:               std::cout << "Type: Other"; break;
    } std::cout << std::endl;
    
    switch (severity)
    {
        case GL_DEBUG_SEVERITY_HIGH:         std::cout << "Severity: high"; break;
        case GL_DEBUG_SEVERITY_MEDIUM:       std::cout << "Severity: medium"; break;
        case GL_DEBUG_SEVERITY_LOW:          std::cout << "Severity: low"; break;
        case GL_DEBUG_SEVERITY_NOTIFICATION: std::cout << "Severity: notification"; break;
    } std::cout << std::endl;
    std::cout << std::endl;
}

bool initGL() {
	printf("%s version: %d.%d\n", CFG_PROJECT_NAME, CFG_VERSION_MAJOR, CFG_VERSION_MINOR);
	
	if (!glfwInit()) {
		fprintf(stderr, "glfwInit() failed.\n");
		return false;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	#if CFG_DEBUG
		// TODO: fix for my window manager
		glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	#endif
	
	/* request debug context */
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, (CFG_DEBUG ? GLFW_TRUE : GLFW_FALSE));
	
	return true;
}

bool initWindow(GLFWwindow **window, int width, int height) {
	*window = glfwCreateWindow(width, height, "main", nullptr, nullptr);
	glfwMakeContextCurrent(*window);
	glfwSwapInterval(1);
	glfwSetFramebufferSizeCallback(*window, onResize);

	if (gl3wInit()) {
		fprintf(stderr, "gl3wInit() failed.\n");
		return false;
	}
	printf("OpenGL %s, GLSL %s\n",
		glGetString(GL_VERSION),
		glGetString(GL_SHADING_LANGUAGE_VERSION));
	
	/* check if debug enabled */
	GLint flags;
	glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
	if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
		printf("OpenGL debug context enabled!\n");
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(glDebugOutput, nullptr);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
	}
	
	return true;
}

GLFWwindow *window = nullptr;
int main(int, char**) {
	
	// disable buffering for stdout (fixes sublime text console)
	#if CFG_DEBUG
		setbuf(stdout, nullptr);
	#endif
	
	/* init gl and window */
	if (!initGL()) fprintf(stderr, "initGL() failed.\n");
	if (!initWindow(&window, 930, 640)) fprintf(stderr, "initWindow() failed.\n");
	
	/* init game */
	World world;
	
	entt::registry engine;
	auto entity = engine.create();
	engine.destroy(entity);
	
	/* draw loop */
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);
		
		glClearColor(0.631f, 0.875f, 0.902f, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		world.draw();
		
		glfwSwapBuffers(window);
	}

	/* cleanup */
	glfwDestroyWindow(window);
	glfwTerminate();
	printf("\ncleanup complete, quitting now...\n\n");
	return 0;
}
