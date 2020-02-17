#pragma once

#include <entt/entt.hpp>
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <ecs/components.hpp>

class CharacterController {
public:
	
	CharacterController(GLFWwindow *window) : window(window) {}
	
	void update(entt::registry &, glm::vec3 viewDir);
	
private:
	GLFWwindow *window;	
};
