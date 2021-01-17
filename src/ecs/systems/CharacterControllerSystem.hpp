#pragma once

#include <memory>

#include <entt/entt.hpp>
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <ecs/components.hpp>

#include <ecs/systems/BaseUpdateSystem.hpp>
#include <RenderObject/Camera.hpp>

class CharacterControllerSystem : public BaseUpdateSystem {
public:
	
	CharacterControllerSystem(entt::registry &registry, GLFWwindow *window, std::shared_ptr<Camera> *camera);
	
	void update();
	
private:
	GLFWwindow *m_window;
	std::shared_ptr<Camera> *m_camera;

};
