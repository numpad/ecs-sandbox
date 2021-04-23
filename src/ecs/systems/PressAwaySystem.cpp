#include <ecs/systems/PressAwaySystem.hpp>

PressAwaySystem::PressAwaySystem(entt::registry &registry)
	: IUpdateSystem(registry) {
	
}

PressAwaySystem::~PressAwaySystem() {
	
}

void PressAwaySystem::update(float dt) {
	auto entities = registry.view<CPosition, CVelocity, const CSphereCollider>();
	entities.each([this, &entities]
		(auto entity, auto &pos, auto &vel, auto &presser) {
			
			glm::vec3 mean_vel(0.0f);
			int count = 0;
			entities.each([this, entity, &pos, &vel, &presser, &mean_vel, &count]
				(auto other_entity, auto &other_pos, auto &other_vel, auto &other_presser) {
					glm::vec3 toMe = pos.pos - other_pos.pos;
					float dp = glm::length2(toMe) / glm::pow(presser.radius + other_presser.radius, 2.0f);
					
					if (entity != other_entity && dp <= 1.0f && dp >= 0.01f) {
						mean_vel += glm::normalize(toMe) * (1.0f - dp) * other_presser.force;
						count++;
						// test
						auto toOther = -toMe;
						other_vel.acc += toOther * 0.1f;
					}
				});
			if (count > 0) {
				mean_vel /= (float)count;
				vel.acc += mean_vel;
			}
		});
}

