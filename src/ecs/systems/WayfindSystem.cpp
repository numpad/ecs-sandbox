#include <ecs/systems/WayfindSystem.hpp>

void WayfindSystem::update(entt::registry &registry) {
	registry.view<CPosition, CRunningToTarget>().each([&registry]
		(auto &entity, auto &pos, auto &runToTarget) {
			glm::vec3 dirToTarget = runToTarget.getTargetPosition(registry) - pos.pos;
			
			if (pos.pos.y == 0.0f && glm::length(dirToTarget) > runToTarget.closeEnough) {
				if (registry.has<CVelocity>(entity)) {
					auto &vel = registry.get<CVelocity>(entity);
					vel.vel += glm::normalize(dirToTarget) * runToTarget.force;
				} else {
					pos.pos += glm::normalize(dirToTarget) * runToTarget.force;
				}
			}
		});
}