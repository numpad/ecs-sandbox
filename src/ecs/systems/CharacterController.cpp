#include <ecs/systems/CharacterController.hpp>

void CharacterController::update(entt::registry &registry, glm::vec3 viewDir) {
	
	registry.view<CPosition, CVelocity, const CKeyboardControllable>().each(
		[this, &registry, viewDir](auto entity,
			auto &pos, auto &vel, const auto &controller)
		{
		
		const glm::vec3 noYAxis(1.0f, 0.0f, 1.0f);
		glm::vec3 viewDirXZ = glm::normalize(noYAxis * viewDir);
		const glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
		
		glm::vec3 dir = glm::vec3(0.0f);
		glm::vec3 right = glm::cross(up, viewDirXZ);
		
		// billboard component
		CBillboard *billboard = nullptr;
		if (registry.has<CBillboard>(entity))
			billboard = &registry.get<CBillboard>(entity);
		
		if (glfwGetKey(this->window, controller.up) == GLFW_PRESS) {
			dir -= viewDirXZ;
		}
		if (glfwGetKey(this->window, controller.down) == GLFW_PRESS) {
			dir += viewDirXZ;
		}
		if (glfwGetKey(this->window, controller.left) == GLFW_PRESS) {
			dir -= right;
			if (billboard) billboard->setFlipped(true);
		}
		if (glfwGetKey(this->window, controller.right) == GLFW_PRESS) {
			dir += right;
			if (billboard) billboard->setFlipped(false);
		}
		
		if (dir.x != 0.0f || dir.y != 0.0f || dir.z != 0.0f) {
			dir = glm::normalize(dir) * controller.speed;
			vel.acc += dir;
		}
	});
}
