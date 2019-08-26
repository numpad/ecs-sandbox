#include <ecs/systems/RandomJumpSystem.hpp>

void RandomJumpSystem::update(entt::registry &registry) {
	static Random randIfJump(0.0f, 1.0f);
	static Random randJumpVel(0.002f, 0.01f);
	registry.view<CJumpTimer, CVelocity, CGravity>().each([&](auto entity, auto &jumper, auto &vel, auto &grav) {
		
		if (abs(vel.vel.y) < 0.00001f && randIfJump() < chanceToJump)
			vel.vel.y += randJumpVel();
	});
}
