#include <ecs/systems/GravitySystem.hpp>

static inline int tilePosRound(float v) {
	v /= 2.f; // tile size
	return (v < 0.0f) ? (int)floor(floor(v) * 0.5f + 0.5f) : int(((float)int(v)) * 0.5f + 0.5f);
}

GravitySystem::GravitySystem(entt::registry &registry, float gravity, Grid2D<SignedDistTerrain> &tileGrid)
	: BaseUpdateSystem(registry), gravity(gravity), tileGrid(tileGrid)
{
	
	registry.ctx<entt::dispatcher>().sink<KillEntityEvent>().connect<&GravitySystem::entityKilled>(*this);
	
}

void GravitySystem::entityKilled(const KillEntityEvent &e) {
	printf("Entity killed: %s\n", e.how.c_str());
	
	if (registry.has<CSpawnPoint>(e.which)) {
		auto [pos, vel] = registry.get<CPosition, CVelocity>(e.which);
		pos.pos = registry.get<CSpawnPoint>(e.which).getPosition(registry);
		vel.vel = glm::vec3(0.0f);
	} else {
		registry.destroy(e.which);
	}
}

void GravitySystem::update() {
	registry.view<CPosition, CVelocity, CGravity>().each([this, &registry = registry](auto entity, auto &pos, auto &vel, auto &gravity) {
			
		if (tileGrid.at(tilePosRound(pos.pos.x), tilePosRound(pos.pos.z)) == nullptr || pos.pos.y > 0.0f)
			vel.acc.y -= this->gravity;
		else if (pos.pos.y <= 0.0f && pos.pos.y >= -0.2f) {
			pos.pos.y = 0.0f;
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
		}
		
		if (pos.pos.y < voidHeight) {
			registry.ctx<entt::dispatcher>().trigger<KillEntityEvent>(entity, "Fell down.");
		}
	});
}
