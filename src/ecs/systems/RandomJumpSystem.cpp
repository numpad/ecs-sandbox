#include <ecs/systems/RandomJumpSystem.hpp>

void RandomJumpSystem::update(entt::registry &registry) {
	static Random randIfJump(0.0f, 100.0f);
	static Random randJumpVel(0.002f, 0.01f);
	registry.view<CVelocity, CGravity>().each([&](auto entity, auto &vel, auto &grav) {
		if (randIfJump() < 1.0f)
			vel.vel.y += randJumpVel();
	});
}
