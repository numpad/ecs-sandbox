#pragma once

#include <entt/entt.hpp>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <ecs/components.hpp>

class CharacterController {
public:
	
	CharacterController(GLFWwindow *window) : window(window) {}
	CharacterController(const CharacterController &copy) = delete;
	
	void update(entt::registry &, glm::vec3 viewPos, glm::vec3 viewDir);
	
private:
	GLFWwindow *window;	
};
