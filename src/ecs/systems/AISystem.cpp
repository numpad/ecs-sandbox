#include <fmt/core.h>
#include <imgui.h>
#include "ecs/systems/AISystem.hpp"
#include "ecs/components/CBehavior.hpp"
#include "ecs/systems/IUpdateSystem.hpp"

AISystem::AISystem(entt::registry& registry) : IUpdateSystem{registry} {
}

void AISystem::update(float dt) {
	registry.view<CBehavior>().each([this](auto entity, auto& cbehavior) {
		cbehavior.behavior->tick(registry, entity);
	});
}
