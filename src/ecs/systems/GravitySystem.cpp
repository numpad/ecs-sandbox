#include <ecs/systems/GravitySystem.hpp>

GravitySystem::GravitySystem(entt::registry &registry, float gravity)
	: IUpdateSystem(registry), gravity(gravity)
{
	
}

void GravitySystem::update(float dt) {
	registry.view<CPosition, CVelocity, CGravity>().each([this, &registry = registry](auto entity, auto &pos, auto &vel, auto &gravity) {
		bool is_grounded = false;

		if (registry.has<CTerrainCollider>(entity)) {
			auto collider = registry.get<CTerrainCollider>(entity);
			is_grounded = collider.is_grounded;

			if (collider.stair_height < collider.max_stair_height && glm::abs(collider.stair_height) > 0.001f) {
				pos.pos.y += collider.stair_height;
			}
		}
		
		if (is_grounded) {
			if (vel.vel.y < 0.0f || vel.acc.y < 0.0f) {
				vel.vel.y = 0.0f;
				vel.acc.y = 0.0f;
				if (abs(vel.vel.y) < 0.00001f) vel.vel.y = 0.0f;
			}
			
			glm::vec2 xz(vel.vel.x, vel.vel.z);
			float xzlen = glm::length(xz);
			glm::vec2 force = -glm::normalize(xz) * 0.0008f;
			if (glm::length(force) < xzlen) {
				vel.acc.x += force.x;
				vel.acc.z += force.y;
			} else {
				vel.vel.x = vel.vel.z = 0.0f;
			}
		} else {
			vel.acc.y -= this->gravity;
		}
	});
}
