#pragma once

#include "config.hpp"
#include <iostream>
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include <RenderObject/Camera.hpp>

class Window {
public:
	
	static bool Init(int glmajor = 4, int glminor = 5);
	static void Destroy();

	bool create(int width, int height);
	void destroy();

	operator GLFWwindow*() const {
		return this->glfw_window;
	}

private:
	GLFWwindow *glfw_window;
};
