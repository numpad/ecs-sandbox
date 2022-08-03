#pragma once

#include <config.hpp>

#include <vector>
#include <functional>
#include <algorithm>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <GLFW/glfw3.h>

#include <Util/Math3d.hpp>

using namespace glm;

class Camera {
public:
	static std::vector<Camera*> CAMERAS;
	static void Init(GLFWwindow* window);

	Camera(vec3 pos, float fov = 32.f);
	Camera(const Camera&) = delete; // no copy constructor
	~Camera();

	// setters
	void setPos(vec3 pos);
	void setTarget(vec3 target);
	void setScreenSize(int width, int height);
	void setFoV(float fov_angles);

	// getters / readonly
	inline vec3 getPos() const {
		return this->position;
	};
	inline vec3 getTarget() const {
		return this->targetpos;
	};
	inline float getZNear() const {
		return this->znear;
	};
	inline float getZFar() const {
		return this->zfar;
	};
	inline vec3 getToTarget() const {
		return this->dirToTarget;
	};
	inline float getFoV() const {
		return glm::degrees(this->fov);
	};
	inline int getScreenWidth() const {
		return this->screen_width;
	};
	inline int getScreenHeight() const {
		return this->screen_height;
	};
	inline vec2 getScreenSize() const {
		return vec2(float(this->screen_width), float(this->screen_height));
	};

	const mat4& getProjection() const;
	const mat4& getHudProjection() const;
	const mat4& getView() const;

	// calculations
	vec2 worldToScreen(vec3 worldpos) const;
	m3d::ray raycast(vec2 normalized_screenpos) const;

	bool windowAspectLocked = true;

private:
	static GLFWwindow* MAIN_WINDOW;

	int screen_width, screen_height;
	float fov, aspect;
	float znear = 1.f, zfar = 100.f;
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
