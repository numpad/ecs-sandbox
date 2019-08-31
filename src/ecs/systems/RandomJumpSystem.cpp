#include <ecs/systems/RandomJumpSystem.hpp>

void RandomJumpSystem::update(entt::registry &registry) {
	static Random randIfJump(0.0f, 1.0f);
	static Random randJumpVel(0.002f, 0.01f);
	registry.view<CPosition, CJumpTimer, CVelocity, CGravity>().each([&](auto entity, auto &pos, auto &jumper, auto &vel, auto &grav) {
		
		if (abs(vel.vel.y) < 0.00001f && randIfJump() < chanceToJump)
			vel.vel.y += randJumpVel();
		
		// jump if running into void
		const float distmult_to_void = 1.75f;
		glm::vec3 np = pos.pos + vel.vel * distmult_to_void;
		glm::vec3 p = pos.pos;
		
		#define INSIDE(P, S) (((P).x >= -S && (P).x <= S) && ((P).z >= -S && (P).z <= S))
		if (vel.vel.y < 0.01f && INSIDE(p, 2.0f) && !INSIDE(np, 2.0f)) {
			vel.vel *= glm::vec3(1.25f, 1.0f, 1.25f);
			vel.vel.y += randJumpVel() + (randJumpVel() * 0.75f);
		}
		#undef INSIDE
	});
}
