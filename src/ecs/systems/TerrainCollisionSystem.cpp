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
		float to_dir = m_chunkedWorld->raycastd(pos.pos + glm::vec3(0.f, 0.02f, 0.f), vel.vel * glm::vec3(1.f, 0.f, 1.f) * 0.1f, 0.1f);

		glm::vec3 maxStairEnd = pos.pos + vel.vel;
		glm::vec3 maxStairStart = maxStairEnd + glm::vec3(0.f, collider.max_stair_height, 0.f);
		float to_stair = m_chunkedWorld->raycastd(maxStairStart, glm::normalize(maxStairEnd - maxStairStart) * 0.001f, collider.max_stair_height * 2.5f);
		collider.is_grounded = (to_floor >= 0.f);
		collider.dist_to_dir = to_dir;
		collider.stair_height = collider.max_stair_height - to_stair;
	});
}
