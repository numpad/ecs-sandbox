#pragma once

#include <config.hpp>

#include <vector>
#include <functional>
#include <algorithm>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <GLFW/glfw3.h>

using namespace glm;

class Camera {
public:
	static std::vector<Camera *> CAMERAS;
	static void Init(GLFWwindow *window);
		
	Camera(vec3 pos, float fov = 32.f);
	~Camera();
	
	void setScreenSize(int width, int height);
	
	const mat4& getHudProjection() const;
	
	bool windowAspectLocked = true;
private:
	static GLFWwindow *MAIN_WINDOW;
	
	int screen_width, screen_height;
	float fov;
	
	vec3 pos;
	
	mat4 projection, view;
	mat4 hudProjection;
	
	void updateHudProj();
};
