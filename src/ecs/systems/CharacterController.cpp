#include <ecs/systems/CharacterController.hpp>

void CharacterController::update(entt::registry &registry,
	glm::vec3 viewPos, glm::vec3 viewDir) {
	
	registry.view<CPosition, CVelocity, const CKeyboardControllable>().each(
		[this, &registry, viewPos, viewDir](auto entity,
			auto &pos, auto &vel, const auto &controller)
		{
		
		const glm::vec3 noYAxis(1.0f, 0.0f, 1.0f);
		glm::vec3 viewDirXZ = glm::normalize(noYAxis * viewDir);
		const glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
		
		glm::vec3 dir = glm::vec3(0.0f);
		glm::vec3 right = glm::cross(up, viewDirXZ);
		
		if (glfwGetKey(this->window, controller.up) == GLFW_PRESS)
			dir -= viewDirXZ;
		if (glfwGetKey(this->window, controller.down) == GLFW_PRESS)
			dir += viewDirXZ;
		if (glfwGetKey(this->window, controller.left) == GLFW_PRESS)
			dir -= right;
		if (glfwGetKey(this->window, controller.right) == GLFW_PRESS)
			dir += right;
		
		if (dir.x != 0.0f || dir.y != 0.0f || dir.z != 0.0f) {
			dir = glm::normalize(dir) * controller.speed;
			vel.vel += dir;
		}
	});
}
