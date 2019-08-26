#include <ecs/systems/PressAwaySystem.hpp>

PressAwaySystem::~PressAwaySystem() {
	
}

void PressAwaySystem::update(entt::registry &registry) {
	auto entities = registry.view<const CPosition, CVelocity, const CPressAway>();
	entities.each([this, &entities]
		(auto entity, auto &pos, auto &vel, auto &presser) {
			
			glm::vec3 mean_vel(0.0f);
			int count = 0;
			entities.each([this, entity, &pos, &vel, &presser, &mean_vel, &count]
				(auto other_entity, auto &other_pos, auto &other_vel, auto &other_presser) {
					glm::vec3 toMe = pos.pos - other_pos.pos;
					float dp = glm::length(toMe) / (presser.radius + other_presser.radius);
					
					if (entity != other_entity && dp <= 1.0f && dp >= 0.01f) {
						mean_vel += glm::normalize(toMe) * (1.1f - dp) * other_presser.force;
						count++;
					}
				});
			if (count > 0) {
				mean_vel /= (float)count;
				vel.vel += mean_vel;
			}
		});
}

