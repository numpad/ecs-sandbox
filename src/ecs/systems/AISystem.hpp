#pragma once

#include "config.hpp"
#include "entt/entt.hpp"
#include "ecs/systems/IUpdateSystem.hpp"
#include "ecs/components.hpp"
#include "ecs/events.hpp"

class AISystem : public IUpdateSystem {
public:
	AISystem(entt::registry& registry);

	void update(float dt);

private:
};
