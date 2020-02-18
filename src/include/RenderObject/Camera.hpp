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
	
	// setters
	void setPos(vec3 pos);
	void setTarget(vec3 target);
	void setScreenSize(int width, int height);
	void setFoV(float fov_angles);
	
	// getters / readonly
	const vec3& pos = position;
	const vec3& target = targetpos;
	const float& zNear = znear;
	const float& zFar = zfar;
	const vec3& toTarget = dirToTarget;
	
	const mat4& getProjection() const;
	const mat4& getHudProjection() const;
	const mat4& getView() const;
	
	bool windowAspectLocked = true;
private:
	static GLFWwindow *MAIN_WINDOW;
	
	int screen_width, screen_height;
	float fov, aspect;
	float znear = .1f, zfar = 1000.f;
	vec3 world_up = vec3(0.f, 1.f, 0.f);
	vec3 position, targetpos;
	
	vec3 dirToTarget;
	
	mat4 projection, view;
	mat4 hudProjection;
	
	void updateHudProj();
	void updateProj();
	void updateView();
	void updateToTarget();
};
