#pragma once

#include <entt/entt.hpp>
#include <ecs/systems/IUpdateSystem.hpp>
#include <ecs/components.hpp>
#include <ecs/events.hpp>

class DamageSystem : public IUpdateSystem {
public:
	DamageSystem(entt::registry& registry);
	~DamageSystem();

	void update(float dt) override;

private:
};
