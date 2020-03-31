#include <ecs/systems/WayfindSystem.hpp>

WayfindSystem::WayfindSystem(entt::registry &registry)
	: BaseUpdateSystem(registry) {
	
}

void WayfindSystem::update() {
	registry.view<CPosition, CRunningToTarget>().each([&registry = registry]
		(auto &entity, auto &pos, auto &runToTarget) {
			glm::vec3 dirToTarget = runToTarget.getTargetPosition(registry) - pos.pos;
			
			if (pos.pos.y == 0.0f && glm::length(dirToTarget) > runToTarget.closeEnough) {
				if (registry.has<CVelocity>(entity)) {
					auto &vel = registry.get<CVelocity>(entity);
					if (glm::length(vel.vel) < 0.009f)
						vel.acc += glm::normalize(dirToTarget) * runToTarget.force;
				} else {
					pos.pos += glm::normalize(dirToTarget) * runToTarget.force;
				}
			} else { // reached target
				if (runToTarget.stopOnceReached) {
					registry.remove<CRunningToTarget>(entity);
					// look a little more alive
					registry.emplace_or_replace<CJumpTimer>(entity);
				}
			}
		});
}
