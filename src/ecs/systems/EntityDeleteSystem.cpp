#include "ecs/systems/EntityDeleteSystem.hpp"

EntityDeleteSystem::EntityDeleteSystem(entt::registry& registry) : IUpdateSystem{registry} {
}

EntityDeleteSystem::~EntityDeleteSystem() {
}

void EntityDeleteSystem::update(float dt) {
	registry.view<CDeletable>().each([this](auto entity, auto& deletable) {
		if (registry.valid(entity)) {
			registry.destroy(entity);
		}
	});
}
