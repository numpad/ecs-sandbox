#include <ecs/systems/DespawnSystem.hpp>

DespawnSystem::DespawnSystem(entt::registry& registry, float voidHeight)
    : IUpdateSystem(registry), voidHeight{voidHeight} {
	registry.ctx<entt::dispatcher>().sink<KillEntityEvent>().connect<&DespawnSystem::entityKilled>(*this);
}

DespawnSystem::~DespawnSystem() {
}

void DespawnSystem::update(float dt) {
	registry.view<CPosition>().each([this](auto entity, auto& pos) {
		// despawn entities who fell into the void
		if (pos.pos.y < voidHeight) {
			registry.ctx<entt::dispatcher>().enqueue<KillEntityEvent>(entity, "Fell down.");
		}
	});
}

void DespawnSystem::entityKilled(const KillEntityEvent& e) {
	// do we need this? what happens if we assign a component to an invalid entity? e.which could already be destroyed
	// when this event is called.
	if (!registry.valid(e.which))
		return;

	if (registry.try_get<CSpawnPoint>(e.which)) {
		auto [pos, vel] = registry.get<CPosition, CVelocity>(e.which);
		pos.pos = registry.get<CSpawnPoint>(e.which).getPosition(registry);
		vel.vel = glm::vec3(0.0f);

		registry.ctx<entt::dispatcher>().enqueue<WorldTextEvent>(e.which, vec3(0.f, .32f, 0.f), L"Ouch...", 60 * 2);

	} else {
		registry.emplace<CDeletable>(e.which);
	}
}
