#pragma once

#include <entt/entt.hpp>
#include <ecs/systems/BaseUpdateSystem.hpp>
#include <ecs/components.hpp>
#include <ecs/events.hpp>

class DespawnSystem : public BaseUpdateSystem {
public:

	DespawnSystem(entt::registry &registry);
	~DespawnSystem();

	void update() override;

	void entityKilled(const KillEntityEvent &e);

private:
	float voidHeight = -5.0f;

};
