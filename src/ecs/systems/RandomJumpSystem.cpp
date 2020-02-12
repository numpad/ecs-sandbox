#include <ecs/systems/RandomJumpSystem.hpp>

void RandomJumpSystem::update() {
	
	static Random randIfJump(0.0f, 1.0f), randIfFlip(0.0f, 1.0f);
	static Random randJumpVel(0.002f, 0.01f);
	
	registry.view<CPosition, CJumpTimer, CVelocity, CGravity>().each(
		[&](auto entity, auto &pos, auto &jumper, auto &vel, auto &grav) {
		
		if (std::abs(vel.vel.y) < 0.00001f && randIfJump() < chanceToJump)
			vel.acc.y += randJumpVel();
		
		// flip texture horizontally
		if (registry.has<CBillboard>(entity) && randIfFlip() < chanceToFlip) {
			auto &bb = registry.get<CBillboard>(entity);
			bb.setFlipped(!bb.isFlipped());
		}
		
	});
}
