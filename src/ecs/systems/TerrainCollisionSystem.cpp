#include <ecs/systems/TerrainCollisionSystem.hpp>

TerrainCollisionSystem::TerrainCollisionSystem(entt::registry &registry, std::shared_ptr<ChunkedWorld> chunkedWorld)
 : BaseUpdateSystem(registry), m_chunkedWorld{chunkedWorld}
{

}

TerrainCollisionSystem::~TerrainCollisionSystem() {

}

void TerrainCollisionSystem::update() {
	registry.view<CPosition, CVelocity, CTerrainCollider>().each([this](auto entity, auto &pos, auto &vel, auto &collider) {
		float d = m_chunkedWorld->raycastd(pos.pos, glm::vec3(0.f, -0.001f, 0.f), 0.001f);
		collider.is_grounded = (d >= 0.f);
	});
}
