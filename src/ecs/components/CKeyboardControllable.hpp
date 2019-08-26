#pragma once

#include <GLFW/glfw3.h>

struct CKeyboardControllable {
	int up = GLFW_KEY_UP,
	    down = GLFW_KEY_DOWN,
	    left = GLFW_KEY_LEFT,
	    right = GLFW_KEY_RIGHT,
	    jump = GLFW_KEY_SPACE;
	
	float speed = 0.01f;
	
	CKeyboardControllable(float speed) : speed(speed)
	{
	}
};
