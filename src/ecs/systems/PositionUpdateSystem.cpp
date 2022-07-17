#include <ecs/systems/PositionUpdateSystem.hpp>

PositionUpdateSystem::PositionUpdateSystem(entt::registry& registry) : IUpdateSystem(registry) {
}

void PositionUpdateSystem::update(float dt) {
	registry.view<CPosition, CVelocity>().each([](auto entity, auto& pos, auto& vel) {
		vel.vel += vel.acc;
		vel.acc *= 0.0f;

		if (glm::length(vel.vel) > vel.maxvel) {
			vel.vel = glm::normalize(vel.vel) * vel.maxvel;
		}

		pos.pos += vel.vel;
	});
}
