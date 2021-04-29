#pragma once

#include <GLFW/glfw3.h>

struct CKeyboardControllable {
	int up = GLFW_KEY_W,
	    down = GLFW_KEY_S,
	    left = GLFW_KEY_A,
	    right = GLFW_KEY_D,
	    jump = GLFW_KEY_SPACE;
	
	float speed = 0.01f;
	
	CKeyboardControllable(float speed) : speed(speed)
	{
	}

	CKeyboardControllable(float speed, int up, int down, int left, int right, int jump)
	 : up(up), down(down), left(left), right(right), jump(jump), speed(speed)
	{
	}
};
