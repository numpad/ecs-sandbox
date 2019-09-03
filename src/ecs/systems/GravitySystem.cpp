#include <ecs/systems/GravitySystem.hpp>

static inline int tilePosRound(float v) {
	return (v < 0.0f) ? (int)floor(floor(v) * 0.5f + 0.5f) : int(((float)int(v)) * 0.5f + 0.5f);
}

GravitySystem::GravitySystem(float gravity)
	: gravity(gravity)
{
}

void GravitySystem::update(entt::registry &registry, const Grid2D<Model> &tileGrid) {
	registry.view<CPosition, CVelocity, CGravity>().each([this, &registry, &tileGrid](auto entity, auto &pos, auto &vel, auto &gravity) {
			
		if (tileGrid.at(tilePosRound(pos.pos.x), tilePosRound(pos.pos.z)) == nullptr || pos.pos.y > 0.0f)
			vel.vel.y -= this->gravity;
		else if (pos.pos.y <= 0.0f) {
			pos.pos.y = 0.0f;
			if (vel.vel.y < 0.0f) {
				vel.vel.y = 0.0f;
				if (abs(vel.vel.y) < 0.00001f) vel.vel.y = 0.0f;
			}
			
			glm::vec2 xz(vel.vel.x, vel.vel.z);
			float xzlen = glm::length(xz);
			glm::vec2 force = -glm::normalize(xz) * 0.0008f;
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
