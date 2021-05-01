#include <ecs/systems/DespawnSystem.hpp>

DespawnSystem::DespawnSystem(entt::registry &registry)
 : IUpdateSystem(registry)
{
	registry.ctx<entt::dispatcher>().sink<KillEntityEvent>().connect<&DespawnSystem::entityKilled>(*this);
}

DespawnSystem::~DespawnSystem() {

}

void DespawnSystem::update(float dt) {
	registry.view<CPosition>().each([this](auto entity, auto &pos) {
		// despawn entities who fell into the void
		if (pos.pos.y < voidHeight) {
			registry.ctx<entt::dispatcher>().trigger<KillEntityEvent>(entity, "Fell down.");
		}
	});
}

void DespawnSystem::entityKilled(const KillEntityEvent &e) {
	registry.ctx<entt::dispatcher>().enqueue<LogEvent>("Entity killed: " + e.how, LogEvent::LOG);
	
	if (registry.has<CSpawnPoint>(e.which)) {
		auto [pos, vel] = registry.get<CPosition, CVelocity>(e.which);
		pos.pos = registry.get<CSpawnPoint>(e.which).getPosition(registry);
		vel.vel = glm::vec3(0.0f);
		
		registry.ctx<entt::dispatcher>().enqueue<WorldTextEvent>(e.which, vec3(0.f, .32f, 0.f), L"Ouch...", 60 * 2);
	
	} else {
		//registry.destroy(e.which);
		registry.emplace<CDeletable>(e.which);
	}
}
