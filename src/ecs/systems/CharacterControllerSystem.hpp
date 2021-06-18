#pragma once

#include <memory>

#include <entt/entt.hpp>
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include <ecs/components.hpp>

#include <ecs/systems/IUpdateSystem.hpp>
#include <RenderObject/Camera.hpp>

class CharacterControllerSystem : public IUpdateSystem {
public:
	
	CharacterControllerSystem(entt::registry &registry, GLFWwindow *window, std::shared_ptr<Camera> *camera);
	
	void update(float dt);
	
private:
	GLFWwindow *m_window;
	std::shared_ptr<Camera> *m_camera;

};
