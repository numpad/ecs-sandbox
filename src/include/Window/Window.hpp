#pragma once

#include "config.hpp"
#include <iostream>
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <RenderObject/Camera.hpp>

class Window {
public:
	enum class FullscreenMode { NONE, WINDOWED, EXCLUSIVE };

	static bool Init(int glmajor = 4, int glminor = 5);
	static void Destroy();

	bool create(int width, int height, FullscreenMode mode = FullscreenMode::NONE);
	void destroy();

	glm::ivec2 getMousePosition() const;
	glm::vec2 getNormalizedMousePosition() const;
	glm::ivec2 getSize() const;

	operator GLFWwindow*() const {
		return this->glfw_window;
	}

private:
	GLFWwindow *glfw_window;
};
