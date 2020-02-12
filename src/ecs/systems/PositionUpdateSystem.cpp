#include <ecs/systems/PositionUpdateSystem.hpp>

PositionUpdateSystem::PositionUpdateSystem(entt::registry &registry)
	: BaseUpdateSystem(registry) {
	
}

void PositionUpdateSystem::update() {
	registry.view<CPosition, CVelocity>().each([](auto entity, auto &pos, auto &vel) {
		const glm::vec3 noY(1.0f, 0.0f, 1.0f);
		
		vel.vel += vel.acc;
		vel.acc *= 0.0f;
		
		if (glm::length(vel.vel * noY) > vel.maxvel) {
			const float prevY = vel.vel.y;
			vel.vel = glm::normalize(vel.vel * noY) * vel.maxvel;
			vel.vel.y = prevY;
		}
		
		pos.pos += vel.vel;
	});
}
