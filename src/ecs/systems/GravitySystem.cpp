#include <ecs/systems/GravitySystem.hpp>

GravitySystem::GravitySystem(float gravity)
	: gravity(gravity)
{
}

void GravitySystem::update(entt::registry &registry) {
	registry.view<CPosition, CVelocity, CGravity>().each([this, &registry](auto entity, auto &pos, auto &vel, auto &gravity) {
		constexpr float dd = 0.5175f;
		if (pos.pos.x < -dd || pos.pos.x > dd || pos.pos.z < -dd || pos.pos.z > dd || pos.pos.y > 0.0f)
			vel.vel.y -= this->gravity;
		else if (pos.pos.y < 0.0f) {
			pos.pos.y = 0.0f;
			vel.vel.y *= -0.35f;
			
			glm::vec2 xz(vel.vel.x, vel.vel.z);
			float xzlen = glm::length(xz);
			glm::vec2 force = -glm::normalize(xz) * 0.0003f;
			if (glm::length(force) < xzlen) {
				vel.vel.x += force.x;
				vel.vel.z += force.y;
			} else {
				vel.vel.x = vel.vel.z = 0.0f;
			}
		}
		
		if (pos.pos.y < -5.0f) registry.destroy(entity);
	});
}
