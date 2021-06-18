#include <ecs/systems/CharacterControllerSystem.hpp>

CharacterControllerSystem::CharacterControllerSystem(entt::registry &registry, GLFWwindow *window, std::shared_ptr<Camera> *camera)
	: IUpdateSystem(registry), m_window{window}, m_camera{camera}
{

}

void CharacterControllerSystem::update(float dt) {
	glm::vec3 viewDir = -((*this->m_camera)->getToTarget());

	registry.view<CPosition, CVelocity, const CKeyboardControllable>().each(
		[this, viewDir](auto entity,
			auto &pos, auto &vel, const auto &controller) {
		
		const glm::vec3 noYAxis(1.0f, 0.0f, 1.0f);
		glm::vec3 viewDirXZ = glm::normalize(noYAxis * viewDir);
		const glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
		
		glm::vec3 dir = glm::vec3(0.0f);
		glm::vec3 right = glm::cross(up, viewDirXZ);
		
		// get optional components
		COrientedTexture *orientation = registry.try_get<COrientedTexture>(entity);
		CBillboard *billboard = registry.try_get<CBillboard>(entity);
		CTerrainCollider *collider = registry.try_get<CTerrainCollider>(entity);

		if (glfwGetKey(this->m_window, controller.up) == GLFW_PRESS) {
			dir -= viewDirXZ;
		}
		if (glfwGetKey(this->m_window, controller.down) == GLFW_PRESS) {
			dir += viewDirXZ;
		}
		if (glfwGetKey(this->m_window, controller.left) == GLFW_PRESS) {
			dir -= right;
		}
		if (glfwGetKey(this->m_window, controller.right) == GLFW_PRESS) {
			dir += right;
		}
		
		if (orientation) {
			orientation->angle = std::atan2(dir.z, dir.x);
		}

		if (collider) {
			if (!collider->is_grounded) {
				dir.x *= 0.f;
				dir.z *= 0.f;
			}
		}

		if (dir.x != 0.0f || dir.y != 0.0f || dir.z != 0.0f) {
			glm::vec3 nv = vel.vel;
			if (glm::length(vel.vel) > 0.001f)
				nv = glm::normalize(vel.vel);
			
			dir = glm::normalize(dir) * controller.speed * 10.f;
			float d = glm::dot(nv, dir);
			vel.vel = dir * (d + .5f) * 5.f;
		} else {
			vel.vel.x *= 0.9f;
			vel.vel.z *= 0.9f;
		}
	});
}
