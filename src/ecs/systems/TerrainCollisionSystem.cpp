#include <ecs/systems/TerrainCollisionSystem.hpp>

TerrainCollisionSystem::TerrainCollisionSystem(entt::registry &registry, std::shared_ptr<ChunkedWorld> chunkedWorld)
 : BaseUpdateSystem(registry), m_chunkedWorld{chunkedWorld}
{

}

TerrainCollisionSystem::~TerrainCollisionSystem() {

}

void TerrainCollisionSystem::update() {
	registry.view<CPosition, CVelocity, CTerrainCollider>().each([this](auto entity, auto &pos, auto &vel, auto &collider) {
		float to_floor = m_chunkedWorld->raycastd(pos.pos, glm::vec3(0.f, -0.001f, 0.f), 0.001f);
		float to_dir = m_chunkedWorld->raycastd(pos.pos + glm::vec3(0.f, 0.01f, 0.f), vel.vel * .1f, glm::length(vel.vel));
		collider.is_grounded = (to_floor >= 0.f);
		collider.dist_to_dir = to_dir;
	});
}
